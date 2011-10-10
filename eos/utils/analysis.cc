/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Frederik Beaujean
 *
 * This file is part of the EOS project. EOS is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * EOS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <eos/utils/analysis.hh>
#include <eos/utils/private_implementation_pattern-impl.hh>
#include <eos/utils/log.hh>

#include <gsl/gsl_cdf.h>

namespace eos
{
   template<>
   struct Implementation<Analysis>
   {
        LogLikelihood log_likelihood;

        Parameters parameters;

        // prior in N dimensions can decouple
        // at most into N 1D priors
        std::vector<LogPriorPtr> priors;

        // Parameter, minimum, maximum, nuisance, discrete
        std::vector<ParameterDescription> parameter_descriptions;

        // names of all parameters. prevent using a parameter twice
        std::set<std::string> parameter_names;

        Implementation(const LogLikelihood & log_likelihood) :
            log_likelihood(log_likelihood),
            parameters(log_likelihood.parameters())
        {
        }

        bool add_parameter(const LogPriorPtr & prior, bool nuisance)
        {
            // clone has correct Parameters object selected
            LogPriorPtr prior_clone = prior->clone(parameters);

            // check if param exists already
            // read out parameters from prior
            for (auto d = prior->begin(), d_end = prior->end() ; d != d_end ; ++d)
            {
                auto result = parameter_names.insert(d->parameter.name());
                if (! result.second)
                    return false;

                d->nuisance = nuisance;
                parameter_descriptions.push_back(*d);
            }

            // then add to prior container
            priors.push_back(prior_clone);

            return true;
        }

        AnalysisPtr clone() const
        {
            // clone log_likelihood
            LogLikelihood llh = log_likelihood.clone();
            AnalysisPtr result = std::make_shared<Analysis>(llh);

            // add parameters via prior clones
            for (auto i = priors.cbegin(), i_end = priors.cend(); i != i_end; ++i)
            {
                result->add((*i)->clone(result->parameters()));
            }

            return result;
        }

        std::pair<double, double>
        goodness_of_fit(const std::vector<double> & parameter_values, unsigned simulated_datasets)
        {
            if (parameter_descriptions.size() != parameter_values.size())
                           throw InternalError("Analysis::goodness_of_fit: starting point doesn't have the correct dimension "
                               + stringify(parameter_descriptions.size()) );

            // set the parameter values
            std::string par_string("( ");
            for (unsigned i=0; i< parameter_values.size(); ++i)
            {
                parameter_descriptions[i].parameter = parameter_values[i];
                par_string += stringify(parameter_values[i]) + " ";
            }
            par_string += ")";

            Log::instance()->message("analysis.goodness_of_fit", ll_informational)
               << "Calculating p-value at parameters " << par_string;

            // calculate \chi^2
            // update observables for new parameter values
            log_likelihood();

            // simulate data sets
            auto sim_result = log_likelihood.bootstrap_p_value(simulated_datasets);

            // p-value from the analytical, yet approximate \chi^2-distribution
            // with (n_obs - n_par) degrees-of-freedom
            unsigned dof = log_likelihood.number_of_observations() - parameter_descriptions.size();
            double chi_squared = gsl_cdf_chisq_Qinv(sim_result.first, log_likelihood.number_of_observations());
            double p_analytical = gsl_cdf_chisq_Q(chi_squared, dof);

            Log::instance()->message("analysis.goodness_of_fit", ll_informational)
               << "p-value after applying DoF correction and using the \\chi^2-distribution"
               << " (valid assumption?) has a value of " << p_analytical;

            return std::make_pair(sim_result.first, p_analytical);
        }

        /*
         * Find index of definition of parameter
         * @param name
         * @return index if found, _parameter_descriptions.size() if not found
         */
        unsigned index(const std::string & name) const
        {
            unsigned result = 0;

            for (auto d = parameter_descriptions.cbegin(), d_end = parameter_descriptions.cend() ; d != d_end ; ++d, ++result)
            {
                if (name == d->parameter.name())
                    return result;
            }

            throw InternalError("Implementation<Analysis>::definition: no such parameter '" + name + "'");
        }

        /*
         * routine needed for optimization. It returns the negative
         * log(posterior) at parameters
         * @param parameters the values of the parameters
         * @param data pointer to an Analysis object
         * @return
         */
        static double
        negative_log_posterior(const gsl_vector * pars, void * data)
        {
            // set all components of parameters
            Implementation<Analysis>* analysis = (Implementation<Analysis>*) data;
            for (unsigned i = 0 ; i < analysis->parameter_descriptions.size() ; ++i)
            {
                analysis->parameter_descriptions[i].parameter = gsl_vector_get(pars, i);
            }

            // calculate negative posterior
            return -(analysis->log_prior() + analysis->log_likelihood());
        }

        bool nuisance(const std::string & name) const
        {
            unsigned index = this->index(name);

            if (index >= parameter_descriptions.size())
            {
                return false;
            }
            else
            {
                return parameter_descriptions[index].nuisance;
            }
        }

        double log_prior()
        {
            if (priors.empty())
                throw InternalError("Analysis::log_prior(): prior is undefined");

            double result = 0.0;

            // all prior components are assumed independent,
            // thus the logs can be simply added up
            for (auto p = priors.cbegin(), p_end = priors.cend() ; p != p_end; ++p)
            {
                result += (**p)();
            }

            return result;
        }

        LogPriorPtr log_prior(const std::string & name) const
        {
            LogPriorPtr prior;

            // loop over all descriptions of the prior pointers
            for (auto p = priors.begin(), p_end = priors.end() ; p != p_end ; ++p)
            {
                for (auto i = (*p)->begin(), i_end = (*p)->end() ; i != i_end ; ++i)
                {
                    if (i->parameter.name() == name)
                        prior = *p;
                }
            }

            return prior;
        }

        std::pair<std::vector<double>, double>
        optimize(const std::vector<double> & initial_guess, const Analysis::OptimizationOptions & options)
        {
            // input validation
            if (parameter_descriptions.size() != initial_guess.size())
               throw InternalError("Analysis::optimize: starting point doesn't have the correct dimension "
                   + stringify(parameter_descriptions.size()) );

            // setup the function object as in GSL manual 36.4
            gsl_multimin_function posterior;
            posterior.n = parameter_descriptions.size();
            posterior.f = &Implementation<Analysis>::negative_log_posterior;
            posterior.params = (void *) this;

            // set starting guess
            gsl_vector *x = gsl_vector_alloc(posterior.n);
            for (unsigned i = 0 ; i < posterior.n ; ++i)
               gsl_vector_set(x, i, initial_guess[i]);

            // save minimum for later comparison
            double initial_minimum = posterior.f(x, (void *) this);

            // set initial step sizes relative to allowed parameter range
            gsl_vector *ss = gsl_vector_alloc(posterior.n);
            for (unsigned i = 0 ; i < posterior.n ; ++i)
               gsl_vector_set(ss, i, (parameter_descriptions[i].max - parameter_descriptions[i].min) * options.initial_step_size);

            // setup the minimizer
            const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2rand;
            gsl_multimin_fminimizer *minim = gsl_multimin_fminimizer_alloc(T, posterior.n);
            gsl_multimin_fminimizer_set(minim, &posterior, x, ss);

            unsigned iter = 0;
            int status = 0;
            double simplex_size = 0;

            // run the minimizer
            do
            {
               iter++;
               status = gsl_multimin_fminimizer_iterate(minim);

               if (status)
                   break;

               simplex_size = gsl_multimin_fminimizer_size(minim);
               status = gsl_multimin_test_size(simplex_size, options.maximum_simplex_size);

               Log::instance()->message("analysis.optimize", ll_debug)
                               << "f() = " << minim->fval << "\tsize = " << simplex_size;

               if (status == GSL_SUCCESS)
               {
                   Log::instance()->message("analysis.optimize", ll_informational)
                       << "Simplex algorithm converged after " << stringify(iter) << " iterations";
               }
            } while (status == GSL_CONTINUE && iter < options.maximum_iterations);

            // build output vector
            std::vector<double> parameters_at_mode(initial_guess);
            double mode = minim->fval;
            for (unsigned i = 0 ; i < posterior.n ; ++i)
               parameters_at_mode[i] = gsl_vector_get(minim->x, i);

            // free resources
            gsl_vector_free(x);
            gsl_vector_free(ss);
            gsl_multimin_fminimizer_free(minim);

            //check if algorithm actually found a better minimum
            if (mode >= initial_minimum)
            {
               Log::instance()->message("analysis.optimize", ll_warning)
                  << "Simplex algorithm did not improve on initial guess";
               return std::make_pair(initial_guess, -initial_minimum) ;
            }

            std::string results("Results: maximum of posterior = ");
            results += stringify(-mode) + " at ( ";
            for (unsigned i = 0 ; i < posterior.n ; ++i)
               results += stringify(parameters_at_mode[i]) + " ";
            results += ")";
            Log::instance()->message("analysis.optimize", ll_informational)
               << results;

            //minus sign to convert to posterior
            return std::make_pair(parameters_at_mode, -mode) ;
        }
    };

    Analysis::Analysis(const LogLikelihood & log_likelihood) :
        PrivateImplementationPattern<Analysis>(new Implementation<Analysis>(log_likelihood))
    {
    }

    Analysis::~Analysis()
    {
    }

    bool
    Analysis::add(const LogPriorPtr & prior, bool nuisance)
    {
        return _imp->add_parameter(prior, nuisance);
    }

    AnalysisPtr
    Analysis::clone() const
    {
        return _imp->clone();
    }

    Parameters
    Analysis::parameters() const
    {
        return _imp->parameters;
    }

    std::pair<double, double>
    Analysis::goodness_of_fit(const std::vector<double> & parameter_values, const unsigned & simulated_datasets)
    {
        return _imp->goodness_of_fit(parameter_values, simulated_datasets);
    }

    LogLikelihood
    Analysis::log_likelihood()
    {
        return _imp->log_likelihood;
    }

    double
    Analysis::log_posterior()
    {
        return _imp->log_prior() + _imp->log_likelihood();
    }

    double
    Analysis::log_prior()
    {
        return _imp->log_prior();
    }

    LogPriorPtr
    Analysis::log_prior(const std::string & name) const
    {
        return _imp->log_prior(name);
    }

    bool
    Analysis::nuisance(const std::string& par_name) const
    {
        return _imp->nuisance(par_name);
    }

    Parameter
    Analysis::operator[] (const unsigned & index)
    {
        return _imp->parameter_descriptions[index].parameter;
    }

    std::pair<std::vector<double>, double>
    Analysis::optimize(const std::vector<double> & initial_guess, const Analysis::OptimizationOptions & options)
    {
        return _imp->optimize(initial_guess, options);
    }

    const std::vector<ParameterDescription> &
    Analysis::parameter_descriptions() const
    {
        return _imp->parameter_descriptions;
    }

    Analysis::OptimizationOptions::OptimizationOptions():
        initial_step_size(0, 1, 0.1),
        maximum_iterations(1000),
        maximum_simplex_size(0, 1, 1e-3)
    {
    }

    Analysis::OptimizationOptions Analysis::OptimizationOptions::Defaults()
    {
        return OptimizationOptions();
    }
}
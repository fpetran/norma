/* Copyright 2013-2015 Marcel Bollmann, Florian Petran
 *
 * This file is part of Norma.
 *
 * Norma is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * Norma is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with Norma.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PLUGINSOCKET_H_
#define PLUGINSOCKET_H_
#include<list>
#include<string>
#include<map>
#include<functional>
#include"string_impl.h"
#include"normalizer/result.h"
#include"normalizer/base.h"

namespace Norma {
class TrainingData;

namespace Normalizer {
class LexiconInterface;
class Base;
}  // namespace Normalizer

/// Apply different Normalizers
/** This class is holds a chain of *Normalizer, and the register of
 *  Normalizer objects. In the future, it should also provide means
 *  of selecting between the results of the Normalizer Result s, which
 *  is currently only based on score returned.
 *
 *  It also takes care of concurrency for normalizing and training,
 *  in that all Normalizer methods are run in parallel, and also
 *  all Normalizer train() methods are run in parallel.
 *
 *  The reasoning behind this is to hide all implementation of the
 *  Normalizer s from the main Cycle, to hide the Cycle logic
 *  from the Normalizer, and most importantly to hide as many
 *  concurrency related issues as possible from the authors of
 *  the Normalizer.
 **/
class PluginSocket : private std::list<Normalizer::Base*> {
 public:
     explicit PluginSocket(const std::string& chain_definition,
                         const std::string& plugin_base_param,
                         const std::map<std::string, std::string>& params);
     PluginSocket() = delete;
     PluginSocket(const PluginSocket& a) = delete;
     const PluginSocket& operator=(const PluginSocket& a) = delete;
     ~PluginSocket();

     /// add a normalizer to the back of the chain
     inline void push_chain(Normalizer::Base* n);
     /// start all normalizers for a word in parallel
     /// then select the best result as soon as all are
     /// ready
     Normalizer::Result normalize(const string_impl& word) const;
     /// start all training in parallel, then wait until
     /// all of them are finished.
     void train(TrainingData *data);
     /// choose the result with the best score
     static const Normalizer::Result&
                best_score(Normalizer::Result* one,
                           Normalizer::Result* two);
     /// choose the result with the lowest priority and score > 0
     static const Normalizer::Result&
                best_priority(Normalizer::Result* one,
                              Normalizer::Result* two);
     void save_params();
     void init_chain();

     typedef std::function<const Normalizer::Result(Normalizer::Result*,
                                                    Normalizer::Result*)>
             Chooser;
     /// choose between two results
     Chooser chooser = PluginSocket::best_priority;

 private:
     Normalizer::Base* create_plugin(const std::string& lib_name,
                                     const std::string& alias = "");
     std::list<std::pair<destroy_t*, Normalizer::Base*>> created_normalizers;
     std::list<void*> loaded_plugins;
     const std::map<std::string, std::string>& config_vars;
     std::string chain_def, plugin_base;
     Normalizer::LexiconInterface* _lex;
};
}  // namespace Norma

#endif  // PLUGINSOCKET_H_


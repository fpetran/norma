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
#include"output.h"
#include<iostream>
#include<string>
#include"normalizer/result.h"

using std::string;

namespace Norma {
//////////////////////////// Output //////////////////////////////////////

Output::Output() {
    _output = &std::cout;
}

void Output::put_line(Normalizer::Result* result,
                      bool print_prob, Normalizer::LogLevel max_level) {
    *_output << result->word;
    if (print_prob)
        *_output << "\t" << result->score;
    *_output << std::endl;
    log_messages(result, max_level);
}

void Output::log_messages(Normalizer::Result* result,
                          Normalizer::LogLevel max_level) {
    while (!result->messages.empty()) {
        Normalizer::LogLevel level;
        std::string origin, message;
        std::tie(level, origin, message) = result->messages.front();
        if (level >= max_level)
            *_output << "[" << Normalizer::level_string(level) << "]:"
                     << message << " Origin: " << origin << std::endl;
        result->messages.pop();
    }
}

//////////////////////////// InteractiveOutput ///////////////////////////

void InteractiveOutput::put_line(Normalizer::Result* result,
                                 bool print_prob,
                                 Normalizer::LogLevel max_level) {
    // can't use Output::put_line here, since what's added
    // to the history is conditional on the validation
    *_output << result->word;
    if (print_prob)
        *_output << "\t" << result->score;
    log_messages(result, max_level);
    *_output << std::endl
             << validate_prompt;
    _opposite->store_last();
    _training->add_target(validate(result->word));
    _request_train = true;
}

std::string InteractiveOutput::validate(const string_impl& line) {
    std::string validate_input;
    getline(std::cin, validate_input);
    if (validate_input.length() != 0)
        return validate_input;
    return to_cstr(line);
}
}  // namespace Norma


//
// Created by loki on 6/20/19.
//

#ifndef OPENTREA_INPUT_H
#define OPENTREA_INPUT_H


namespace input {

struct input_t;

void print(void *input);
void passthrough(std::shared_ptr<input_t> &input, std::vector<std::uint8_t> &&input_data);

void init();

std::shared_ptr<input_t> alloc();
}

#endif //OPENTREA_INPUT_H

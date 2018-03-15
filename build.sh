#!/usr/bin/env bash

gcc \
  -std=c99 \
  -O3 \
  -Wall -Wextra \
  -Wno-missing-braces \
  -lglfw \
  -framework OpenGL \
  -fwrapv \
  -fno-strict-aliasing \
  source/*.c \
  -o cogsim

#!/usr/bin/env bash

gcc \
  -std=c99 \
  -Wall -Wextra \
  -lglfw \
  -framework OpenGL \
  -Wno-missing-braces \
  source/*.c \
  -o cogsim

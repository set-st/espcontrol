"""ESPHome external component stub for espcontrol.

Registers this directory as an include path so C++ headers (button_grid.h,
icons.h, sun_calc.h) are available to lambdas in device YAML configs.
No YAML schema — all config is handled by the YAML packages.
"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.core import coroutine_with_priority
import os

CODEOWNERS = ["@set-st"]

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    comp_dir = os.path.dirname(os.path.abspath(__file__))
    cg.add_build_flag(f"-I{comp_dir}")

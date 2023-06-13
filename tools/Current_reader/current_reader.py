#!/usr/bin/env python3

import pathlib


class current_reader:
    TELEMETRY_LENGTH = 132

    def __init__(self, dirname: str, runID: int) -> None:
        self.runID: int = runID
        self.path = pathlib.Path(dirname)
        

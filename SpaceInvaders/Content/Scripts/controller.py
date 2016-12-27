import sys
import time

import json
import requests

import unreal_engine as ue
import numpy as np

SEED = 42

# Logging
LOG_PATH = r"D:\Denis\Logs"  # path to logs
LOG_TIMINGS = False  # Whether to log controller speed on every tick

class UnrealEngineOutput:
    def __init__(self, logger):
        self.logger = logger

    def write(self, buf):
        self.logger(buf)

    def flush(self):
        return


sys.stdout = UnrealEngineOutput(ue.log)
sys.stderr = UnrealEngineOutput(ue.log_error)

ue.log("Python version: {}".format(sys.version))


class PythonAIController(object):

    # Called at the started of the game
    def begin_play(self):
        ue.log("Begin Play on PythonAIController class")
        np.random.seed(SEED)

    def get_screen(self, game_mode):
        if not game_mode:
            return None

        screen_capturer = game_mode.ScreenshotCapturer
        screenshot = np.array(screen_capturer.Screenshot)
        H = screen_capturer.Height
        W = screen_capturer.Width
 
        if len(screenshot) == 0:
            return None
 
        return screenshot.reshape((H, W, 3), order='F').swapaxes(0, 1)#.astype(np.uint8)

    def get_score(self, game_mode):
        if not game_mode:
            return (0, 0)
        return (game_mode.AliveEnemies, game_mode.Lives)

    # Called periodically during the game
    def tick(self, delta_seconds: float):        
        start_time = time.clock()

        game_mode = self.uobject.GameMode
        score = self.get_score(game_mode)        
        screen = self.get_screen(game_mode)

        # Skip frames when no screen is available
        if screen is None or len(screen) == 0:
            return

        # Send params to neural network
        r = requests.post('http://127.0.0.1:8081/neural_network', 
                          data=json.dumps(
                              {'score': score, 
                               'screen': screen.tolist()}))
        
        # Receive action from neural network
        action = json.loads(r.text)['action']
        
        # Completing action
        if action < 3:
            game_mode.Fire = False
            game_mode.MovementDirection = action-1
        else:
            game_mode.MovementDirection = action-4
            game_mode.Fire = True

        # Log elapsed time
        finish_time = time.clock()
        elapsed = finish_time - start_time
        if LOG_TIMINGS:
            ue.log("Delta seconds: {}, time elapsed: {}".format(delta_seconds, elapsed))
   
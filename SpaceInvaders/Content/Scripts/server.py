from aiohttp import web
import asyncio
import json
import logging
import sys
import time

import numpy as np
import tensorflow as tf
import cv2

from agent_trainer import AgentTrainer

sys.setrecursionlimit(100000)

# Model
GAME = "SpaceInvaders"
ACTIONS = 6  # number of valid actions
MODEL_PATH = r"D:\Denis\saved_networks"  # path to saved models
SNAPSHOT_PERIOD = 10000  # periodicity of saving current model
TOTAL_SHIPS = 55
SHIPS = 55
SEED = 42

config = {
    "action_count": ACTIONS,
    "gamma": 0.99,  # decay rate of past observations
    "observe_step_count": 64,  # timesteps to observe before training
    "explore_step_count": 50000,  # frames over which to anneal epsilon
    "initial_epsilon": 0.00001,  # starting value of epsilon
    "final_epsilon": 0.0001,  # final value of epsilon
    "replay_memory_size": 100000,  # number of previous transitions to remember
    "match_memory_size": 1000,  # number of previous matches to remember
    "batch_size": 64,  # size of minibatch
    "frame_per_action": 1,  # ammount of frames that are skipped before every action
    "log_period": 100,  # periodicity of logging
}


loop = asyncio.get_event_loop()
log = logging.getLogger("game")


def calc_score(x):
    return (TOTAL_SHIPS - x[0]) * 10

class Score(object):
    def __init__(self, ships_alive, player_lives):
        self.score = (ships_alive, player_lives)

    def update(self, new_score):
        reward = -0.1
        
        if self.score != new_score:
            if new_score[0] == 0:
                reward += 1000
                TOTAL_SHIPS += SHIPS
            if new_score[1] == 0:
                TOTAL_SHIPS = SHIPS
                reward -= 350            
            if new_score[1] < self.score[1]:
                reward -= 10 * (3 - new_score[1])
            reward += calc_score(new_score) - calc_score(self.score)
            self.score = new_score
        return reward


async def ws_msg_collector(request):
    global trainer, current_score, step_count
    #global pic_i

    data = await request.json()
    #log.info("Received: %s", data)

    # parse message
    score, screen = data['score'], np.array(data['screen'])

    # Calculate reward
    reward = current_score.update(score)
    
    # Save picture
    #cv2.imwrite("D:\\Denis\\Screenshots\\screen{}.png".format(pic_i), 255.0 * screen)
    #pic_i += 1
    #if pic_i == 1000:
    #    pic_i = 0
        
    # Process frame
    trainer.process_frame(screen.astype(np.uint8), reward, reward != 0)            

    # Make new action
    action = trainer.act()            

    # Send new action
    to_send = json.dumps({'action': int(action)})
    
    # Save model
    step_count += 1
    if step_count % SNAPSHOT_PERIOD == 0:
        trainer.save_model(MODEL_PATH)
        log.info("Saving model")

    return web.Response(text=to_send)

    
async def start_server():
    app = web.Application()
    app.router.add_route("POST", "/neural_network", ws_msg_collector)
    server = await loop.create_server(app.make_handler(), '0.0.0.0', 8081)

    return server


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format="%(asctime)s [%(levelname)s]: %(message)s")
    log.info("Starting server")
    
    trainer = AgentTrainer(config)
    trainer.init_training()
    trainer.load_model(MODEL_PATH)
    step_count = 0
    current_score = Score(0, 0)
    #pic_i = 0
    
    np.random.seed(SEED)
    tf.set_random_seed(SEED)    

    loop.run_until_complete(start_server())
    loop.run_forever()

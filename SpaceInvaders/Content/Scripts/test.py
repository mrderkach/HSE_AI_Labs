# -*- coding: utf-8 -*-

import sys
import time

import aiohttp
import asyncio
import json
import requests

import numpy as np


#@asyncio.coroutine
async def foo():
    print(1)
    session = aiohttp.ClientSession()
    async with session.post('https://127.0.0.1:8081/neural_network', data=json.dumps({'score': (1,0), 'screen': [0, 0, 0]})) as resp:
        print(await resp.text())
        
    print(1)
    return 1
    
from requests.adapters import HTTPAdapter
from requests.packages.urllib3.poolmanager import PoolManager
import ssl

class MyAdapter(HTTPAdapter):
    def init_poolmanager(self, connections, maxsize, block=False):
        self.poolmanager = PoolManager(num_pools=connections,
                                       maxsize=maxsize,
                                       block=block,
                                       ssl_version=ssl.PROTOCOL_TLSv1)
        
#loop = asyncio.get_event_loop()

#loop.run_until_complete(foo)
#asyncio.get_event_loop().run_until_complete(foo)

#coros = []
#coros.append(foo())
#loop = asyncio.get_event_loop()
#loop.run_until_complete(asyncio.wait(coros))


#s = requests.Session()
#s.mount('https://', MyAdapter())
r = requests.post('http://127.0.0.1:8081/neural_network', data=json.dumps({'score': (1,0), 'screen': [0, 0, 0]}))
print(r.text)
#print(''.join(**json.loads(r.text)).encode('utf-8'))

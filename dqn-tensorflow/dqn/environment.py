import gym
import random
import numpy as np
from .utils import to_grayscale, crop_image, imresize

from PIL import Image

class Environment(object):
  def __init__(self, config):
    self.env = gym.make(config.env_name)

    screen_width, screen_height, self.action_repeat, self.random_start = \
        config.screen_width, config.screen_height, config.action_repeat, config.random_start

    self.display = config.display
    self.dims = (screen_width, screen_height)
    
    #////////////////////////////////////////////////////////////////////////////////
    
    self.env_name = config.env_name
    
    #////////////////////////////////////////////////////////////////////////////////
    
    self._screen = None
    self.reward = 0
    self.terminal = True

  def new_game(self, from_random_game=False):
    if self.lives == 0:
      self._screen = self.env.reset()
    self._step(0)
    self.render()
    return self.screen, 0, 0, self.terminal

  def new_random_game(self):
    self.new_game(True)
    for _ in xrange(random.randint(0, self.random_start - 1)):
      self._step(0)
    self.render()
    return self.screen, 0, 0, self.terminal

  def _step(self, action):
    self._screen, self.reward, self.terminal, _ = self.env.step(action)

  def _random_step(self):
    action = self.env.action_space.sample()
    self._step(action)


  #////////////////////////////////////////////////////////////////////////////////

  def process_image(self, image):
    if self.env_name == "Breakout-v0":            #BREAKOUT
      img = crop_image(image, 0, 18, 84, 102)
      img = to_grayscale(img).astype(np.uint8)
      #img = img / 255.
      #img = to_grayscale(img).astype(np.uint8)
      """for i in range(84):
        for j in range(84):
          print img[i][j]"""
      #print "*************", img.shape, "*************"
      #print ">>>>>>>>>>>>>", img.dtype, ">>>>>>>>>>>>>"
      #imagen = Image.fromarray(img, 'L')
      #imagen = imagen.resize((840, 840), Image.ANTIALIAS)
      #imagen.show()
      #exit()
      return img
    elif self.env_name == "SpaceInvaders-v0":     #SPACE INVADERS
      img = crop_image(image, 0, 18, 84, 102)
      img = to_grayscale(img).astype(np.uint8)
      return img
    elif self.env_name == "BeamRider-v0":     #BEAM RIDER
      img = crop_image(image, 0, 21, 84, 105)
      img = to_grayscale(img).astype(np.uint8)
      return img
    elif self.env_name == "Frostbite-v0":     #FROSTBITE
      img = crop_image(image, 0, 16, 84, 100)
      img = to_grayscale(img).astype(np.uint8)
      return img
    elif self.env_name == "Pong-v0":    #PONG
      img = crop_image(image, 0, 18, 84, 102)
      img = to_grayscale(img).astype(np.uint8)
      return img
    elif self.env_name == "Qbert-v0":    #Q* BERT
      img = crop_image(image, 0, 16, 84, 100)
      img = to_grayscale(img).astype(np.uint8)
      return img
    elif self.env_name == "StarGunner-v0":    #STAR GUNNER
      img = crop_image(image, 0, 14, 84, 98)
      img = to_grayscale(img).astype(np.uint8)
      return img
    else:
      return imresize(to_grayscale(self._screen)/255., self.dims)       #ORIGINAL

  @ property
  def screen(self):
    return self.process_image(self._screen)          #CROP
    #return imresize(to_grayscale(self._screen)/255., self.dims)       #ORIGINAL

  #////////////////////////////////////////////////////////////////////////////////

  @property
  def action_size(self):
    return self.env.action_space.n

  @property
  def lives(self):
    return self.env.ale.lives()

  @property
  def state(self):
    return self.screen, self.reward, self.terminal

  def render(self):
    if self.display:
      self.env.render()

  def after_act(self, action):
    self.render()

class GymEnvironment(Environment):
  def __init__(self, config):
    super(GymEnvironment, self).__init__(config)

  def act(self, action, is_training=True):
    cumulated_reward = 0
    start_lives = self.lives

    for _ in xrange(self.action_repeat):
      self._step(action)
      cumulated_reward = cumulated_reward + self.reward

      if is_training and start_lives > self.lives:
        cumulated_reward -= 1
        self.terminal = True

      if self.terminal:
        break

    self.reward = cumulated_reward

    self.after_act(action)
    return self.state

class SimpleGymEnvironment(Environment):
  def __init__(self, config):
    super(SimpleGymEnvironment, self).__init__(config)

  def act(self, action, is_training=True):
    self._step(action)

    self.after_act(action)
    return self.state

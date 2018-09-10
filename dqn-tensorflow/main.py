from __future__ import print_function
import random
import tensorflow as tf

from dqn.agent import Agent
from dqn.environment import GymEnvironment, SimpleGymEnvironment
from config import get_config

flags = tf.app.flags

# Model
flags.DEFINE_string('model', 'm1', 'Type of model')
flags.DEFINE_boolean('dueling', False, 'Whether to use dueling deep q-network')
flags.DEFINE_boolean('double_q', False, 'Whether to use double q-learning')

# Environment
flags.DEFINE_string('env_name', 'Breakout-v0', 'The name of gym environment to use')
flags.DEFINE_integer('action_repeat', 4, 'The number of action to be repeated')

# Etc
flags.DEFINE_boolean('use_gpu', True, 'Whether to use gpu or not')
flags.DEFINE_string('gpu_fraction', '1/1', 'idx / # of gpu fraction e.g. 1/3, 2/3, 3/3')
flags.DEFINE_boolean('display', False, 'Whether to do display the game screen or not')
flags.DEFINE_string('mode', 'train', 'Whether to do training or testing')
flags.DEFINE_integer('random_seed', 123, 'Value of random seed')

#///////////////////////////////////////////////////////
#cualquier parametro de dqn que se quiera pasar por parametro (en lugar de ponerlo en
#"config.py") se pone aqui y se modifica el metodo "get_config()" de "config.py" para 
#admitirlo como parametro

#dqn + ale
#flags.DEFINE_integer('memory_size', 300000, 'Tamano de la replay memory')
#flags.DEFINE_integer('ep_end_t', 1000000, 'Frames necesarios para epsilon minimo')
#flags.DEFINE_float('learning_rate', 0.00025, 'Ratio de aprendizaje')

#///////////////////////////////////////////////////////

FLAGS = flags.FLAGS

# Set random seed
tf.set_random_seed(FLAGS.random_seed)
random.seed(FLAGS.random_seed)

if FLAGS.gpu_fraction == '':
  raise ValueError("--gpu_fraction should be defined")

def calc_gpu_fraction(fraction_string):
  idx, num = fraction_string.split('/')
  idx, num = float(idx), float(num)

  fraction = 1 / (num - idx + 1)
  print(" [*] GPU : %.4f" % fraction)
  return fraction

def main(_):
  gpu_options = tf.GPUOptions(
      per_process_gpu_memory_fraction=calc_gpu_fraction(FLAGS.gpu_fraction))

  with tf.Session(config=tf.ConfigProto(gpu_options=gpu_options)) as sess:
    config = get_config(FLAGS) or FLAGS

    if config.env_type == 'simple':
      env = SimpleGymEnvironment(config)
    else:
      env = GymEnvironment(config)

    if not tf.test.is_gpu_available() and FLAGS.use_gpu:
      raise Exception("use_gpu flag is true when no GPUs are available")

    if not FLAGS.use_gpu:
      config.cnn_format = 'NHWC'

    agent = Agent(config, env, sess)

    if FLAGS.mode == "train":
      agent.train()
    elif FLAGS.mode == "test":
      agent.play()
    elif FLAGS.mode == "ale":
      agent.play2()

if __name__ == '__main__':
  tf.app.run()

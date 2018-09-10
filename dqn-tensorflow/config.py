class AgentConfig(object):
  scale = 10000
  display = False

  max_step = 5000 * scale                       #5000 * scale         #modificado
  memory_size = 30 * scale                    #100 * scale          #modificado

  batch_size = 32
  random_start = 30
  cnn_format = 'NCHW'
  discount = 0.99
  target_q_update_step = 1 * scale
  learning_rate = 0.00025
  learning_rate_minimum = 0.00025
  learning_rate_decay = 0.96
  learning_rate_decay_step = 5 * scale

  ep_end = 0.1
  ep_start = 1.
  ep_end_t = 100 * scale                             #memory_size     #modificado

  history_length = 4
  train_frequency = 4
  learn_start = int(2.5 * scale)                   #5. * scale         #modificado

  min_delta = -1
  max_delta = 1

  double_q = False
  dueling = False

  _test_step = int(2.5 * scale)                    #5 * scale    #modificado
  _save_step = _test_step * 1                     #_test_step * 10       #modificado

class EnvironmentConfig(object):
  env_name = 'Breakout-v0'

  screen_width  = 84
  screen_height = 84
  max_reward = 1.
  min_reward = -1.

class DQNConfig(AgentConfig, EnvironmentConfig):
  model = ''
  pass

class M1(DQNConfig):
  backend = 'tf'
  env_type = 'detail'
  action_repeat = 1

def get_config(FLAGS):
  if FLAGS.model == 'm1':
    config = M1
  elif FLAGS.model == 'm2':
    config = M2

  for k, v in FLAGS.__dict__['__flags'].items():  #FLAGS son las que se han definido en main.py
    if k == 'gpu':
      if v == False:
        config.cnn_format = 'NHWC'
      else:
        config.cnn_format = 'NCHW'
        
    #//////////////////////////////////////////////////////////////////////    
    """
    if k == "memory_size":
      config.memory_size = v
    if k == "ep_end_t":
      config.ep_end_t = v
    if k == "learning_rate":
      config.learning_rate = v
      config.learning_rate_minimum = v
    """
    #//////////////////////////////////////////////////////////////////////

    if hasattr(config, k):  #aqui se comprueba que el argumento de nombre "k" exista
      setattr(config, k, v) #aqui se establece el valor por defecto del argumento "k", definido en main.py  

  return config

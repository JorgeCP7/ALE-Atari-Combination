import time
import numpy as np
import tensorflow as tf
import sys
if (sys.version_info[0]==2):
  import cPickle
elif (sys.version_info[0]==3):
  import _pickle as cPickle

"""try:
  from scipy.misc import imresize
except:
  import cv2
  imresize = cv2.resize"""
  
  
#////////////////////////////////////////////////////////////////////////////////////////
  
from PIL import Image
from scipy.misc import imresize
  
#recibe array RGB y lo transforma a array GRAY 
def to_grayscale(image):
  return np.dot(image[...,:3], [0.299, 0.587, 0.114])
  
#recibe un array, lo transforma a Image, realiza resize y crop, y lo transforma de nuevo a array
def crop_image(image, x1, y1, x2, y2):
  image = Image.fromarray(image)
  image = image.resize((84, 110), Image.ANTIALIAS)
  image = image.crop((x1, y1, x2, y2))
  image = np.array(image, np.uint8)
  return image
  
#////////////////////////////////////////////////////////////////////////////////////////


def timeit(f):
  def timed(*args, **kwargs):
    start_time = time.time()
    result = f(*args, **kwargs)
    end_time = time.time()

    print("   [-] %s : %2.5f sec" % (f.__name__, end_time - start_time))
    return result
  return timed

def get_time():
  return time.strftime("%Y-%m-%d_%H:%M:%S", time.gmtime())

@timeit
def save_pkl(obj, path):
  with open(path, 'w') as f:
    cPickle.dump(obj, f)
    print("  [*] save %s" % path)

@timeit
def load_pkl(path):
  with open(path) as f:
    obj = cPickle.load(f)
    print("  [*] load %s" % path)
    return obj

@timeit
def save_npy(obj, path):
  np.save(path, obj)
  print("  [*] save %s" % path)

@timeit
def load_npy(path):
  obj = np.load(path)
  print("  [*] load %s" % path)
  return obj

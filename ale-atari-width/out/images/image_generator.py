#!/usr/bin/python
import os
import sys
import numpy as np

from PIL import Image

def main(argv):

  if not os.path.exists(argv[1]):
    os.makedirs(argv[1])

  path = argv[1] + "/" + argv[2] + ".png"
  
  pathname = os.path.dirname(sys.argv[0])    
    
  if pathname == "":
    fichero = open("image-data.txt")
  else:
    fichero = open(pathname + "/image-data.txt")

  
  screen = np.zeros((210*160*3), dtype=np.uint8)
      
  #Llenar screen
  i = 0
  while i < screen.size:
    screen[i] = int(fichero.readline())
    i+=1

  
  screen = np.reshape(screen, (210,160,3))
  img = Image.fromarray(screen, "RGB")
  
  img.save(path)
  
  
main(sys.argv)

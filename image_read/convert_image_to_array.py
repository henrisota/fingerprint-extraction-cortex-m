import numpy as np
from PIL import Image

arr = np.asarray(Image.open('101_1.bmp'))

np.savetxt('101_1.csv', arr, '%3.d', delimiter=',')

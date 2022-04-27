import numpy as np
from PIL import Image

fingerprint_data = np.genfromtxt('binary_image_out.csv', delimiter=',')
fingerprint_data = np.reshape(fingerprint_data, (300, 300))

img = Image.fromarray(np.uint8(fingerprint_data), 'L')
img.save('fingerprint.bmp')

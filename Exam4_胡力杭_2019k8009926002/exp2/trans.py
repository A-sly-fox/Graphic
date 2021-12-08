from PIL import Image
img = Image.open('trace.ppm')
img.save('trace.bmp')
img.show()

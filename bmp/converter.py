from PIL import Image

img = Image.open("FILE_NAME_HERE.bmp")
img = img.convert('RGB')
arr = list(img.getdata())
parsed_list = [int(num) for tpl in arr for num in tpl]
print(parsed_list)

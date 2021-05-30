#生成GIF图
import imageio
import cv2
import os
def compress_image(im, max_size=1000000):
    im_shape = list(im.shape)
    while im_shape[0] * im_shape[1] > max_size:
        im_shape[0] //= 2
        im_shape[1] //= 2
    im = cv2.resize(im, (im_shape[1], im_shape[0]))
    return im

def create_gif(gif_name, pic_path, duration=0.3, compress=True):
    """
    生成gif文件
    gif_name ： 字符串，所生成的 gif 文件名，带 .gif 后缀
    pic_path :      需要合成为 gif 的图片所在路径
    duration :  gif 图像时间间隔
    compress: 是否压缩图像
    """
    pic_suffixes = {'.png', '.jpg', '.jpeg', '.bmp'}
    frames = []
    png_files = sorted(os.listdir(pic_path))
    image_list = [os.path.join(pic_path, f) for f in png_files]
    for image_name in image_list:
        if os.path.splitext(image_name.lower())[-1] in pic_suffixes:
            print('add picture from:', image_name)
            im = imageio.imread(image_name)
            if compress:
                # 图片太大则对图片压缩
                im = compress_image(im)
            frames.append(im)
    # 保存为 gif
    if frames:
        print('saving gif to:', gif_name)
        imageio.mimsave(gif_name, frames, 'GIF', duration=duration)
    else:
        print('There is no supporting picture in the path', pic_path,
              '. Only support the pictures with suffixes by', str(pic_suffixes))




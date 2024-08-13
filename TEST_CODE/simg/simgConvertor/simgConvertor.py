import os
from PIL import Image
from struct import pack

def convert_rgb_to_rgb565(r, g, b):
    """ RGB를 RGB565로 변환 """
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def convert_rgb_to_bgr565(r, g, b):
    """ RGB를 BGR565로 변환 """
    return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3)


def rgb565_to_bytes(rgb565):
    """ RGB565를 little endian 포멧으로 변경. """
    return pack('<H', rgb565)

def process_image(image_path):
    """ 이미지를, 두가지 압축방식으로 압축하고 모두 반환"""
    image = Image.open(image_path)
    rgb_image = image.convert('RGB')
    pixels = list(rgb_image.getdata())

    # Method 1: RGB565 for each pixel
    # method1_data = b''.join([rgb565_to_bytes(convert_rgb_to_rgb565(*p)) for p in pixels])

    # Method 1: RGB565 for each pixel
    method1_data = b''.join([rgb565_to_bytes(convert_rgb_to_bgr565(*p)) for p in pixels])

    # Method 2: Compress continuous pixels
    method2_data = b''
    prev_pixel = None
    count = 0

    for pixel in pixels:
        rgb565 = convert_rgb_to_rgb565(*pixel)
        if prev_pixel is None or prev_pixel != rgb565:
            if prev_pixel is not None:
                method2_data += rgb565_to_bytes(prev_pixel) + pack('B', count)
            prev_pixel = rgb565
            count = 1
        else:
            if count < 255:
                count += 1
            else:
                method2_data += rgb565_to_bytes(prev_pixel) + pack('B', count)
                count = 1

    # Add the last pixel
    if prev_pixel is not None:
        method2_data += rgb565_to_bytes(prev_pixel) + pack('B', count)

    return image.width, image.height, method1_data, method2_data

def print_simg_content(data):
    """ 출력 .simg 파일의 바이트를 정수로 출력"""
    for byte in data:
        print(byte, end=' ')
    print()  # New line at the end

def save_and_print_simg(image_path, output_folder):
    """ 처리후 저장, .simg 이미지 정보 출력 """
    width, height, method1_data, method2_data = process_image(image_path)

    # 용량이 적은 압축 방식 선택
    if len(method1_data) < len(method2_data):
        data = method1_data
        compression_flag = 1
    else:
        data = method2_data
        compression_flag = 2


    # 해더 구성 (가로픽셀-2byte ,세로픽셀-2byte,압축방식-1byte)
    header = pack('<HHB', width, height, compression_flag)

    # .simg 파일 저장
    output_path = os.path.join(output_folder, os.path.splitext(os.path.basename(image_path))[0] + '.simg')
    with open(output_path, 'wb') as f:
        f.write(header + data)

    # 정보저장
    print(f"image path   : {image_path}")
    print(f"Method 1 size: {len(method1_data)} bytes")
    print(f"Method 2 size: {len(method2_data)} bytes")
    print(f"Chosen compression method: {compression_flag}")

    # .simg 내용을 정수로 출력
    print_simg_content(header + data)

    return output_path


def process_folder(folder_path):
    """ folder_path 폴더의 모든 이미지를 처리하고 /result 폴더에 저장합니다."""
    result_folder = os.path.join(folder_path, 'result')
    if not os.path.exists(result_folder):
        os.makedirs(result_folder)

    for filename in os.listdir(folder_path):
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
            image_path = os.path.join(folder_path, filename)
            save_and_print_simg(image_path, result_folder)

    return result_folder

# 이미지가 담긴 폴더 위치 지정
folder_path = '/Users/laptop/Desktop/im/'
result_folder = process_folder(folder_path)

print(f"Processed images saved in {result_folder}")


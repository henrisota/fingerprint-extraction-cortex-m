import cv2
import numpy as np
import os
import shutil
import subprocess

from math import cos, sin, pi
from PIL import Image, ImageOps, ImageDraw

# Inverted red
RIDGE_ENDING_COLOR = (0, 255, 255)
RIDGE_ENDING_COLOR_INVERTED = (255 - RIDGE_ENDING_COLOR[0], 255 - RIDGE_ENDING_COLOR[1], 255 - RIDGE_ENDING_COLOR[2])
# Inverted blue
RIDGE_BIFURCATION_COLOR = (255, 255, 0)
RIDGE_BIFURCATION_COLOR_INVERTED = (
    255 - RIDGE_BIFURCATION_COLOR[0], 255 - RIDGE_BIFURCATION_COLOR[1], 255 - RIDGE_BIFURCATION_COLOR[2])


ORIENTATION_LINE_LENGTH = 5

FINGERPRINTS_DIRECTORY = 'fingerprints'
OUTPUT_DIRECTORY = 'fingerprints_processed'

FINGERPRINT_EXTRACTION_PROJECT_DIRECTORY = 'fr_me_local'
FINGERPRINT_EXTRACTION_PROJECT_BINARY = 'main'
FINGERPRINT_EXTRACTION_PROJECT_BUILD_FILE = 'binary_image_out.csv'
FINGERPRINT_EXTRACTION_PROJECT_MINUTIAE_FILE = 'fingerprint_minutiae.csv'

IMAGE_READING_PROJECT_DIRECTORY = 'image_read'
IMAGE_READING_PROJECT_BINARY = 'main'
IMAGE_READING_PROJECT_BUILD_FILE = 'binary_image.gbf'


def clean_output_directory():
    # Delete directory with all its contents if it already exists
    try:
        shutil.rmtree(OUTPUT_DIRECTORY)
    except OSError as e:
        print(f'Error: {OUTPUT_DIRECTORY}: {e.strerror}')

    # Create directory
    try:
        os.mkdir(OUTPUT_DIRECTORY)
    except OSError:
        print(f'Creation of directory {OUTPUT_DIRECTORY} failed')
        exit()
    else:
        print(f'Successfully created directory {OUTPUT_DIRECTORY}')


def list_fingerprint_images():
    files = list()

    local_directory_entries = os.listdir(FINGERPRINTS_DIRECTORY)

    for entry in local_directory_entries:
        path = os.path.join(FINGERPRINTS_DIRECTORY, entry)

        if path.endswith('.bmp'):
            files.append(path)

    return files


def perform_extraction_and_overlay(source):
    # Load image to have it ready for overlaying later
    base_fingerprint = cv2.imread(source)

    # Move image to image reading project
    source_name = os.path.basename(source)
    destination = os.path.join(IMAGE_READING_PROJECT_DIRECTORY, source_name)
    shutil.copyfile(source, destination)

    # Change to image reading project
    os.chdir(os.path.join('.', IMAGE_READING_PROJECT_DIRECTORY))

    # Run image reading project
    args = (os.path.join('.', IMAGE_READING_PROJECT_BINARY), source_name)

    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()

    # Remove image from image reading project
    os.remove(source_name)

    # Copy build image to fingerprint extraction project
    destination = os.path.join('..', FINGERPRINT_EXTRACTION_PROJECT_DIRECTORY, IMAGE_READING_PROJECT_BUILD_FILE)
    shutil.copyfile(IMAGE_READING_PROJECT_BUILD_FILE, destination)

    # Remove build image from image reading project
    os.remove(IMAGE_READING_PROJECT_BUILD_FILE)

    # Change directory to fingerprint extraction project
    os.chdir(os.path.join('..', FINGERPRINT_EXTRACTION_PROJECT_DIRECTORY))

    # Run fingerprint extraction project
    args = (os.path.join('.', FINGERPRINT_EXTRACTION_PROJECT_BINARY), IMAGE_READING_PROJECT_BUILD_FILE)

    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()

    # Move build file to output directory
    output_file_name = os.path.splitext(source_name)[0] + '.csv'
    destination = os.path.join('..', OUTPUT_DIRECTORY, output_file_name)
    shutil.copyfile(FINGERPRINT_EXTRACTION_PROJECT_BUILD_FILE, destination)

    # Move minutiae extraction build file to output directory
    minutiae_output_file_name = os.path.splitext(source_name)[0] + '_minutiae.csv'
    destination = os.path.join('..', OUTPUT_DIRECTORY, minutiae_output_file_name)
    shutil.copyfile(FINGERPRINT_EXTRACTION_PROJECT_MINUTIAE_FILE, destination)

    # Remove input file to fingerprint extraction project
    os.remove(IMAGE_READING_PROJECT_BUILD_FILE)

    # Remove build file from fingerprint extraction project
    os.remove(FINGERPRINT_EXTRACTION_PROJECT_BUILD_FILE)

    # Remove minutiae extraction build file from fingerprint extraction project
    os.remove(FINGERPRINT_EXTRACTION_PROJECT_MINUTIAE_FILE)

    # Change to output directory
    os.chdir(os.path.join('..', OUTPUT_DIRECTORY))

    # Load fingerprint data
    fingerprint_data = np.genfromtxt(output_file_name, delimiter=',')
    fingerprint_data = np.reshape(fingerprint_data, (300, 300))

    # Create image from the fingerprint data
    img = Image.fromarray(np.uint8(fingerprint_data), 'L')
    output_image_file_name = os.path.splitext(output_file_name)[0] + '.bmp'
    img.save(output_image_file_name)

    # Load the image into cv2
    overlay_fingerprint = cv2.imread(output_image_file_name)

    # Create original image on the output directory for comparison
    cv2.imwrite(source_name, base_fingerprint)

    # Create processed image on the output directory for comparison
    cv2.imwrite(f'{os.path.splitext(source_name)[0]}_processed.bmp', overlay_fingerprint)

    # Create overlaid image
    cv2.imwrite(f'{os.path.splitext(output_file_name)[0]}_overlaid.bmp',
                cv2.addWeighted(base_fingerprint, 1.0, overlay_fingerprint, 0.4, 0))

    # Load extracted minutiae data
    minutiae_data = np.genfromtxt(minutiae_output_file_name, delimiter=',')

    extracted_minutiae_img = Image.fromarray(np.zeros((300, 300)), 'RGB')
    extracted_minutiae_img_pixels = extracted_minutiae_img.load()

    for minutiae in minutiae_data:
        if minutiae[3] == 1:
            # Ridge ending minutiae
            extracted_minutiae_img_pixels[minutiae[1], minutiae[0]] = RIDGE_ENDING_COLOR
        else:
            # Ridge bifurcation minutiae
            extracted_minutiae_img_pixels[minutiae[1], minutiae[0]] = RIDGE_BIFURCATION_COLOR

    # Invert image
    extracted_minutiae_img = ImageOps.invert(extracted_minutiae_img)

    extracted_minutiae_img_file_name = f'{os.path.splitext(source_name)[0]}_minutiae_extracted.png'
    extracted_minutiae_img.save(extracted_minutiae_img_file_name)

    # Create overlaid image with extracted minutiae
    cv2.imwrite(f'{os.path.splitext(output_file_name)[0]}_minutiae_overlaid.png', cv2.addWeighted(
        base_fingerprint, 0.8, cv2.imread(extracted_minutiae_img_file_name), 0.4, 0))

    draw = ImageDraw.Draw(extracted_minutiae_img)

    # Mark minutiae with orientation line
    for entry in minutiae_data:
        pos_x = entry[0]
        pos_y = entry[1]

        length = ORIENTATION_LINE_LENGTH

        if (pos_x - length >= 0 and pos_x + length < 300 and pos_y >= 0 and pos_y + length < 300):
            dx = length * cos((entry[2]) * pi / 180)
            dy = length * sin((entry[2]) * pi / 180)
            draw.line([pos_y, pos_x, pos_y + dy, pos_x + dx], fill=128)

    pixels = extracted_minutiae_img.load()

    # Draw minutiae points again on top of the lines
    for entry in minutiae_data:
        pixels[entry[1], entry[0]] = RIDGE_ENDING_COLOR_INVERTED if entry[3] == 1 else RIDGE_BIFURCATION_COLOR_INVERTED

    extracted_minutiae_with_orientation_img_file_name = f'{os.path.splitext(source_name)[0]}_minutiae_orientation_extracted.png'
    extracted_minutiae_img.save(extracted_minutiae_with_orientation_img_file_name, 'png')

    # Create overlaid image with extracted minutiae and orientation
    cv2.imwrite(f'{os.path.splitext(source_name)[0]}_minutiae_orientation_overlaid.png', cv2.addWeighted(
        base_fingerprint, 0.8, cv2.imread(extracted_minutiae_with_orientation_img_file_name), 0.4, 0))

    # Move back to main directory
    os.chdir('..')


if __name__ == '__main__':
    clean_output_directory()

    images = list_fingerprint_images()

    for image in images:
        perform_extraction_and_overlay(image)

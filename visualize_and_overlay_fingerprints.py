import cv2
import numpy as np
import os
import shutil
import subprocess

from PIL import Image

FINGERPRINTS_DIRECTORY = 'fingerprints'
OUTPUT_DIRECTORY = 'fingerprints_processed'

FINGERPRINT_EXTRACTION_PROJECT_DIRECTORY = 'fr_me_local'
FINGERPRINT_EXTRACTION_PROJECT_BINARY = 'main'
FINGERPRINT_EXTRACTION_PROJECT_BUILD_FILE = 'binary_image_out.csv'

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
    print(source_name)
    shutil.copyfile(source, destination)

    # Change to image reading project
    os.chdir(os.path.join('.', IMAGE_READING_PROJECT_DIRECTORY))

    # Run image reading project
    args = (os.path.join('.', IMAGE_READING_PROJECT_BINARY), source_name)
    print(args)

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
    print(args)

    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()

    # Move build file to output directory
    output_file_name = os.path.splitext(source_name)[0] + '.csv'
    destination = os.path.join('..', OUTPUT_DIRECTORY, output_file_name)
    shutil.copyfile(FINGERPRINT_EXTRACTION_PROJECT_BUILD_FILE, destination)

    # Remove input file to fingerprint extraction project
    os.remove(IMAGE_READING_PROJECT_BUILD_FILE)

    # Remove build file from fingerprint extraction project
    os.remove(FINGERPRINT_EXTRACTION_PROJECT_BUILD_FILE)

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
                cv2.addWeighted(base_fingerprint, 0.8, overlay_fingerprint, 0.2, 0))

    # Move back to main directory
    os.chdir('..')


if __name__ == '__main__':
    clean_output_directory()

    images = list_fingerprint_images()

    for image in images:
        perform_extraction_and_overlay(image)

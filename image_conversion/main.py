import zipfile
import os
import shutil

from PIL import Image


def unzip(path, destination='./'):
    with zipfile.ZipFile(path, 'r') as zip_ref:
        zip_ref.extractall(destination)


def convert_image(in_path, out_path):
    Image.open(in_path).save(out_path)


def get_list_of_files(directory, extensions):
    files = list()

    local_directory_entries = os.listdir(directory)

    for entry in local_directory_entries:
        path = os.path.join(directory, entry)

        if os.path.isdir(path):
            files += get_list_of_files(path, extensions)
        else:
            if path.endswith(extensions):
                files.append(path)

    return files


if __name__ == '__main__':
    directory = 'databases'
    extensions = ('.zip')

    databases = get_list_of_files(directory, extensions)

    out_directory_path = f'./unzipped_{directory}'

    # Delete directory with all its contents if it already exists
    try:
        shutil.rmtree(out_directory_path)
    except OSError as e:
        print(f'Error: {out_directory_path}: {e.strerror}')

    # Create director
    try:
        os.mkdir(out_directory_path)
    except OSError:
        print(f'Creation of directory {out_directory_path} failed')
    else:
        print(f'Successfully created directory {out_directory_path}')

    for database in databases:
        database_name = os.path.splitext(database)[0]
        unzip(database, out_directory_path + '/' + database_name)

    images = get_list_of_files(out_directory_path, ('.tif'))

    for image_path in images:
        converted_image_path = os.path.splitext(image_path)[0] + '.bmp'

        convert_image(image_path, converted_image_path)

        try:
            os.remove(image_path)
        except OSError as e:
            print(f'Error: {image_path}: {e.strerror}')

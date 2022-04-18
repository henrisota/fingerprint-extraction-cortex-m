import os
import requests
import shutil
import zipfile

from PIL import Image


def fstr(template):
    return eval(f"f'{template}'")


def download(url, out_path):
    # Create directories to place file in case they do not exist
    os.makedirs(os.path.dirname(out_path), exist_ok=True)

    try:
        with open(out_path, 'wb+') as f:
            req = requests.get(url, allow_redirects=True)
            f.write(req.content)
    except:
        os.remove(out_path)


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
    url_template = 'http://bias.csr.unibo.it/{competition}/Downloads/{db}'
    competitions = ['fvc2000', 'fvc2002', 'fvc2004']
    dbs = [f'DB{i}_B.zip' for i in range(1, 5)]
    directory = 'databases'
    extensions = ('.zip')

    if not os.path.exists(directory):
        try:
            os.mkdir(directory)
        except OSError:
            print(f'Creation of directory {directory} failed')
            exit()
        else:
            print(f'Successfully created directory {directory}')

    for competition in competitions:
        for db in dbs:
            url = fstr(url_template)
            out_path = f'{directory}/{competition.upper()}/{competition.upper()}_{db}'

            if not os.path.exists(out_path):
                download(url, out_path)

    databases = get_list_of_files(directory, extensions)

    out_directory_path = f'./unzipped_{directory}'

    # Delete directory with all its contents if it already exists
    try:
        shutil.rmtree(out_directory_path)
    except OSError as e:
        print(f'Error: {out_directory_path}: {e.strerror}')

    # Create directory
    try:
        os.mkdir(out_directory_path)
    except OSError:
        print(f'Creation of directory {out_directory_path} failed')
        exit()
    else:
        print(f'Successfully created directory {out_directory_path}')

    for database in databases:
        database_name = '/'.join(os.path.splitext(database)[0].split('/')[1:])
        unzip(database, out_directory_path + '/' + database_name)

    images = get_list_of_files(out_directory_path, ('.tif'))

    for image_path in images:
        converted_image_path = os.path.splitext(image_path)[0] + '.bmp'
        convert_image(image_path, converted_image_path)

        try:
            os.remove(image_path)
        except OSError as e:
            print(f'Error: {image_path}: {e.strerror}')

#!/usr/bin/env python

from __future__ import print_function
import argparse
import errno
import hashlib
import os
import shutil
import sys
import tempfile
# Python 3 / 2 compat import
try:
  from urllib.request import urlopen
except ImportError:
  from urllib2 import urlopen

sys.path.append(
  os.path.abspath(os.path.dirname(os.path.abspath(__file__)) + "/../.."))

DIST_URL = 'https://github.com/stack-inc/stack-electron/releases/download/'


def rm_rf(path):
  try:
    shutil.rmtree(path)
  except OSError:
    pass

def safe_mkdir(path):
  try:
    os.makedirs(path)
  except OSError as e:
    if e.errno != errno.EEXIST:
      raise

def download(text, url, path):
  safe_mkdir(os.path.dirname(path))
  with open(path, 'wb') as local_file:
    print("Downloading %s to %s" % (url, path))
    web_file = urlopen(url)
    info = web_file.info()
    if hasattr(info, 'getheader'):
      file_size = int(info.getheaders("Content-Length")[0])
    else:
      file_size = int(info.get("Content-Length")[0])
    downloaded_size = 0
    block_size = 4096

    while True:
      buf = web_file.read(block_size)
      if not buf:
        break

      downloaded_size += len(buf)
      local_file.write(buf)
      percent = downloaded_size * 100. / file_size
      status = "\r%s  %10d  [%3.1f%%]" % (text, downloaded_size, percent)
      print(status, end=' ')

    print()
  return path


def main():
  args = parse_args()
  dist_url = args.dist_url
  if dist_url[-1] != "/":
    dist_url += "/"

  url = dist_url + args.version + '/'
  directory, files = download_files(url, get_files_list(args.version))
  checksums = [
    create_checksum('sha1', directory, 'SHASUMS.txt', files),
    create_checksum('sha256', directory, 'SHASUMS256.txt', files)
  ]

  if args.target_dir is not None:
    copy_files(checksums, args.target_dir)

  rm_rf(directory)


def parse_args():
  parser = argparse.ArgumentParser(description='upload sumsha file')
  parser.add_argument('-v', '--version', help='Specify the version',
                      required=True)
  parser.add_argument('-u', '--dist-url',
                      help='Specify the dist url for downloading',
                      required=False, default=DIST_URL)
  parser.add_argument('-t', '--target-dir',
                      help='Specify target dir of checksums',
                      required=False)
  return parser.parse_args()

def get_files_list(version):
  return [
    { "filename": 'electron-api.json', "required": True },
    { "filename": 'electron-{0}-darwin-arm64.zip'.format(version), "required": True },
    { "filename": 'electron-{0}-darwin-x64.zip'.format(version), "required": True },
    { "filename": 'electron-{0}-win32-x64.zip'.format(version), "required": True },
    { "filename": 'electron.d.ts', "required": True },
  ]


def download_files(url, files):
  directory = tempfile.mkdtemp(prefix='electron-tmp')
  result = []
  for optional_f in files:
    required = optional_f['required']
    f = optional_f['filename']
    try:
      result.append(download(f, url + f, os.path.join(directory, f)))
    except Exception:
      if required:
        raise

  return directory, result


def create_checksum(algorithm, directory, filename, files):
  lines = []
  for path in files:
    h = hashlib.new(algorithm)
    with open(path, 'rb') as f:
      h.update(f.read())
      lines.append(h.hexdigest() + ' *' + os.path.relpath(path, directory))

  checksum_file = os.path.join(directory, filename)
  with open(checksum_file, 'w') as f:
    f.write('\n'.join(lines) + '\n')
  return checksum_file

def copy_files(source_files, output_dir):
  for source_file in source_files:
    output_path = os.path.join(output_dir, os.path.basename(source_file))
    safe_mkdir(os.path.dirname(output_path))
    shutil.copy2(source_file, output_path)

if __name__ == '__main__':
  sys.exit(main())

FROM nvcr.io/nvidia/l4t-tensorflow:r32.7.1-tf2.7-py3

WORKDIR /home/greenbarber
COPY . .

RUN pip3 install -r requirements.txt

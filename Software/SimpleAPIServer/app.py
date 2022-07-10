import base64
import json
import os
import random
import string
import subprocess
from wsgiref.simple_server import make_server

CONTENTS_PATH = "contents"
IMAGE_PATH = "image"
EDGE_FILTER_PATH = os.path.join(os.environ["HOME"], "bmp", "bmp_filter")

# function to get random filename when temporary saving original image
def get_random_bmp_filename(n):
    randlst = [random.choice(string.ascii_letters + string.digits) for i in range(n)]
    return ''.join(randlst) + ".bmp"


def get_json_containing_edge_detected_image(request_json):
    original_image_base64 = request_json["base64_image"].replace(' ', '+')
    raw_original_image = base64.b64decode(original_image_base64)
    # Step1. retrieve image data from json and save to temporary file
    try:
        random_filename = get_random_bmp_filename(20)
        original_image_filepath = os.path.join(IMAGE_PATH, random_filename)
        original_image_file = open(original_image_filepath, mode="wb")
        original_image_file.write(raw_original_image)
    except:
        print("Error occurred in reading original image")
        return
    finally:
        original_image_file.close()
    # Step2. execute edge detection
    edge_detected_image_filepath = original_image_filepath + "out"
    process = subprocess.call(["convert", original_image_filepath, "-type", "truecolor", original_image_filepath])
    if process != 0:
        print(process)
        print("Error occurred in image conversion")
        return
    process = subprocess.call([EDGE_FILTER_PATH, original_image_filepath, edge_detected_image_filepath])
    if process != 0:
        print(process)
        print("Error occurred in executing edge detection")
        return
    # Step3. convert edge-detected image to json string
    try:
        edge_detected_image_file = open(edge_detected_image_filepath, mode="rb")
        edge_detected_image = edge_detected_image_file.read()
        edge_detected_image_base64 = base64.b64encode(edge_detected_image).decode()
    except:
        print("Error occurred in creating base64 string for response")
        return
    finally:
        edge_detected_image_file.close()
    return {"base64_edge_detected_image": edge_detected_image_base64}


def on_html(environ, start_response):
    path = environ["PATH_INFO"]
    headers = [('Content-type', 'text/html; charset=utf-8')]
    if path in ("", "/"):
        path = os.path.join(CONTENTS_PATH, "index.html")
        with open(path, encoding="utf-8") as html:
            status = '200 OK'
            start_response(status, headers)
            return [html.read().encode(encoding="utf-8")]
    else:
        status = '404 Not Found'
        start_response(status, headers)
        return [bytes(path, encoding="utf-8")]


def on_api(environ, start_response):
    headers = [
        ('Content-type', 'application/json; charset=utf-8'),
        ('Access-Control-Allow-Origin', '*')
    ]
    path = environ["PATH_INFO"]
    request_method = environ["REQUEST_METHOD"]
    if request_method == "POST":
        status = '200 OK'
        wsgi_input = environ["wsgi.input"]
        content_length = int(environ["CONTENT_LENGTH"])
        request_json = json.loads(wsgi_input.read(content_length).decode())
        response_json = get_json_containing_edge_detected_image(request_json)
        start_response(status, headers)
        return [json.dumps(response_json).encode("utf-8")]
    else:
        status = '400 Bad Request'
        start_response(status, headers)
        return [bytes(path, encoding="utf-8")]


def app(environ, start_response):
    path = environ["PATH_INFO"]
    if path.startswith("/post"):
        return on_api(environ, start_response)
    else:
        return on_html(environ, start_response)


if __name__ == '__main__':
    try:
        httpd = make_server('', 3000, app)
        print("Serving on port 3000...")
        httpd.serve_forever()
    except Exception as e:
        print(e)
    finally:
        httpd.shutdown()
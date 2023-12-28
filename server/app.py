from genericpath import isfile
from math import cos
from flask import Flask, request, make_response, jsonify, render_template
import json, os

# import ndjson

app = Flask(__name__)

try:
    app.json.ensure_ascii = False           # flask version >= 3.0.0
except:
    app.config['JSON_AS_ASCII'] = False     # flask version < 3.0.0


class const:
    latest_data_path =  "data/m5stick_tennis_latest_data.json"
    prev_data_path = "data/m5stick_tennis_prev_data.json"




@app.route("/m5stick_tennis/post", methods=["POST"])
def m5stick_tennis_post():
    data = request.json
    data = json.dumps(data)
    print("data",data,flush=True)
    prev_data = ""
    os.makedirs("./data", exist_ok=True)
    is_file = os.path.isfile(const.latest_data_path)
    print("is_file:", is_file)
    if is_file:
        with open(const.latest_data_path) as f:
            prev_data = f.read()
            print("prev_data",prev_data,flush=True)

    with open(const.latest_data_path, mode="w") as f:
        
        f.write(data)

    with open(const.prev_data_path, mode="w") as f:
        f.write(prev_data)
    
    return {"prevdata":prev_data}



@app.route("/m5stick_tennis/data")
def m5stick_tennis_data():
    latest_data = '{}'
    prev_data = "{}"
    os.makedirs("./data", exist_ok=True)       
    if os.path.isfile(const.latest_data_path): 
        with open(const.latest_data_path, mode="r") as f:
            latest_data = f.read()
    if os.path.isfile(const.latest_data_path):
        with open(const.prev_data_path, mode="r") as f:
            prev_data = f.read()

    return {"latest_data":latest_data, "prev_data":prev_data}


@app.route("/m5stick_tennis/view")
def view():
    
    return render_template("m5stick_tennis_view.html")

@app.route("/health/text")
def health_text():
    return "health_text\n"

# import datetime
# @app.route("/health/get/response")
# def health_get_response():
#     response_text = make_response(("hogeほげ", "200"))
#     # response_text.headers.setlist("Content-Type", ["text/html; charset=ascii","text/plain; charset=utf-8"])
#     # response_text.content_type = "text/plain; charset=utf-8"
#     print("response_text:",vars(response_text))
#     print("response_text.get_data():",response_text.get_data(as_text=False))      # as_text=True:(非エスケープ) hogeほげ, as_text=False:(エスケープ) b'hoge\xe3\x81\xbb\xe3\x81\x92'
#     # print(f'response_text.headers: <{response_text.headers}>,\nresponse_text.headers[Content-Type]: <{response_text.headers["Content-Type"]}>\n')
#     print("response_text.response",response_text.response, type(response_text.response))

#     # response_json = make_response(jsonify({"hogeほげ":3}))
#     response_json = make_response({"hogeほげ":3})
#     print("response_json:",vars(response_json))
#     print("response_json.get_data():",response_json.get_data(as_text=True))
#     print(f'response_json.headers: <{response_json.headers}>,\nresponse_json.headers["Content-Type"]: <{response_json.headers["Content-Type"]}>\n')
#     print("response_json.response", response_json.response, type(response_json.response))

#     # response_json["Content-Type"] = "application/json; charset=utf-8"     # このようにすることはできない(このやり方ではRead-onlyらしい。 `TypeError: 'Response' object does not support item assignment`)
#     response_json.content_type = "application/json; charset=utf-8"
#     response_json.headers["Content-Type"] = "application/json; charset=utf-8"
#     print("response_json.get_data():",response_json.get_data(as_text=True))
#     response_json.content_length = 90       # 変更できる
#     response_json.date = datetime.date(2002,11,11)
#     # response_json.status_code = 200
#     # response_json.status = "200 OK"
#     print("status:",response_json.status)
#     # response_json.headers["Date"] = datetime.date(2002,11,11)
#     # response_json.connection = "open"
#     print(dir(response_json), response_json.json, response_json.date, response_json.data)
#     print(dir(response_json.headers))
#     # response_json.headers.add("Date", datetime.date(2002,11,11))
#     print(vars(response_json.headers))
#     # print(f'response_json.headers: <{response_json.headers}>,\nresponse_json.headers["Content-Type"]: <{response_json.headers["Content-Type"]}>\n')
#     return response_text


@app.route("/health/post/text", methods=["POST"])
def health_post_text():
    req  = request
    print(vars(req))
    data = req.get_data()
    print("data, type:",data, type(data))   # data, type: b'hoge' <class 'bytes'>
    # return f"posted data is {data}"          
    return data


@app.route("/health/post/json", methods=["POST"])
def health_post_json():
    req = request
    req_is_json = req.is_json
    req_json = req.get_json()
    print(vars(req), req_json)
    return {"status":req_is_json}







if __name__ == "__main__":
    app.run(debug=True)  # デバッグモードがオンになり、変更があるとリロードされ変更が適用される。

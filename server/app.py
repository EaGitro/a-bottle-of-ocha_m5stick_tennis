from genericpath import isfile
from math import cos
from flask import Flask, request
import json, os
# import ndjson

app = Flask(__name__)

class const:
    latest_data_path =  "data/m5stick_tennis_latest_data.json"
    prev_data_path = "data/m5stick_tennis_prev_data.json"




@app.route("/m5stick_tennis/post", methods=["POST"])
def m5stick_tennis_post():
    data = request.json
    data = json.dumps(data, ensure_ascii=False, indent=2)
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
    
    return f"OK. prevdata:{prev_data}"



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

    return f"{{latest_data:{latest_data}, prev_data:{prev_data}}}" 


@app.route("/m5stick_tennis/view")
def view():
    accelerations = ""
    f = open(const.latest_data_path, "r")
    accelerations = f.read()
    return f"{accelerations}"


if __name__ == "__main__":
    app.run(debug=True)  # デバッグモードがオンになり、変更があるとリロードされ変更が適用される。

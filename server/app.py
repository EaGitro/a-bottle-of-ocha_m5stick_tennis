from flask import Flask, request
# import ndjson

app = Flask(__name__)


@app.route("/m5stick_tennis/post", method=["POST"])
def m5stick_tennis_post():
    data = request.json
    prev_data = ""
    with open("latest_data.json", mode="r+") as f:
        prev_data = f.read()
        f.write(data)

    with open("prev_data.json", mode="r") as f:
        f.write(prev_data)


@app.route("/m5stick_tennis/data")
def m5stick_tennis_data():
    latest_data = ""
    prev_data = ""
    with open("latest_data.json", mode="r") as f:
        latest_data = f.read()
    with open("prev_data.json", mode="r") as f:
        prev_data = f.read()

    return f"{{latest_data:{latest_data}, prev_data:{prev_data}}}" 


@app.route("/m5stick_tennis/view")
def view():
    accelerations = ""
    f = open("data.ndjson", "r")
    accelerations = f.read()
    return f"{accelerations}"


if __name__ == "__main__":
    app.run(debug=True)  # デバッグモードがオンになり、変更があるとリロードされ変更が適用される。

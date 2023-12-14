from flask import Flask
app = Flask(__name__)


global_accelerations = ""

@app.route('/m5stick/<accelerations>')
def acceleration(accelerations):
    f = open('myfile.txt', 'w')
    f.write(accelerations)
    f.close()
    return f'<p>{accelerations}</p>'



@app.route('/view')
def view():
    accelerations = ""
    f = open("myfile.txt", "r")
    accelerations = f.read()
    return f'{accelerations}'

if __name__ == "__main__":
    app.run(debug=True)         # デバッグモードがオンになり、変更があるとリロードされ変更が適用される。



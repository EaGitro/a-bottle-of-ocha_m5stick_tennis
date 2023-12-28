let isFetching = false;

let loopId = 0;

let intervalTime = 500;

const latestData = {
    term_cnt: -1
};


let sameDataCnt = 0;       // if sameDataCnt > 5; stop fetch.

/**
 *  与えられたデータの中から新たなデータを抽出し、その配列とdelayの値を返します。
 * @param {*} data 
 * @returns { {"dataArr": any[], "delay": number} }
 */
function extractNewData(data) {
    console.log("extractnewnode. data:", data);
    let newDataArr = [];
    let delay = 0;
    for (let prevOrLatest in data) {
        let termCnt = data[prevOrLatest].term_cnt;
        delay = data[prevOrLatest].delay;
        if (termCnt > latestData.term_cnt) {
            newDataArr.push(...data.prev_data.moments)
            latestData.term_cnt = termCnt;
        }
    }
    return { "dataArr": newDataArr, "delay": delay }
}


/**
 *  与えられた1つのデータを文字列にしてDOMに追加します
 * @todo ここを変えてください
 * @param {any[]} data 新たなデータの配列
 * @param {HTMLElement} elm 追加するHTMLELElement
 */
function addSingleNode(data, elm) {
    console.log("addsinglenode");
    let newElm = document.createElement("p");
    newElm.innerText = JSON.stringify(data);
    elm.appendChild(newElm);
}

/**
 * 与えられたミリ秒だけ非同期的にsleepさせる関数
 * @param {number} ms ミリ秒
 */
async function sleep(ms) {
    await new Promise(resolve => setTimeout(resolve, ms))
}

/**
 * 新しいデータを受け取り、そこから配列を抽出して、delay秒づつ addSingleNode() に渡します
 * @see {@link addSingleNode} 新しい配列をこれに渡します
 * @see {@link sleep} 与えられた秒だけ待つ関数です
 * @param {any} dataArr 新しいデータ
 * @param {number} delay 遅延
 * @param {HTMLElement} elm 追加するDOM
 */
async function addNodes(dataArr, delay, elm) {
    const startTime = Date.now();
    console.log("addNode:", delay)
    for (let data of dataArr) {
        await sleep(delay + 2);
        addSingleNode(data, elm);
    }
    const endTime = Date.now();
    console.log(`${endTime - startTime}秒経過`)
}

/**
 * データを要求し、それを用いてDOM操作をします。
 * @param {URL} endpoint fetchするエンドポイントです
 * 
 */
async function operateNode(endpoint) {
    console.log("operateNode");
    let elm = document.querySelector("#show-data");
    // const response = await fetch(endpoint);
    // const fetchedJson = await response.json()
    const fetchedJson
        = '{"prev_data":'
        + '{"term_cnt":6,"freq":10,"delay":50,"moments":[{"cnt":0,"ms":14911,"acc":[-0.38623, -0.13843, 0.94824],"gyro":[37.96387, 123.53516, -0.97656]},{"cnt":1,"ms":14962,"acc":[-0.57373, -0.22095, 0.73975],"gyro":[12.14600, 65.06348, -5.31006]},{"cnt":2,"ms":15012,"acc":[-0.57593, -0.24268, 0.80322],"gyro":[22.52197, 41.19873, 20.75195]},{"cnt":3,"ms":15062,"acc":[-0.64478, -0.13379, 0.92017],"gyro":[17.70020, 36.07178, 14.28223]},{"cnt":4,"ms":15112,"acc":[-0.61377, -0.06641, 0.72388],"gyro":[37.90283, 14.70947, 15.99121]},{"cnt":5,"ms":15162,"acc":[-0.58911, -0.13281, 0.74756],"gyro":[-10.98633, 13.06152, 6.53076]},{"cnt":6,"ms":15212,"acc":[-0.67480, -0.15576, 0.86646],"gyro":[-1.03760, 22.64404, 7.69043]},{"cnt":7,"ms":15262,"acc":[-0.59033, -0.10645, 0.89185],"gyro":[-1.64795, 9.33838, 9.39941]},{"cnt":8,"ms":15312,"acc":[-0.70630, -0.14038, 0.79834],"gyro":[2.99072, 23.25439, 7.38525]},{"cnt":9,"ms":15362,"acc":[-0.66382, -0.13623, 0.85327],"gyro":[-9.82666, 20.56885, 1.95312]}]}'
        + ', "latest_data":'
        + '{"term_cnt":7,"freq":10,"delay":50,"moments":[{"cnt":0,"ms":16036,"acc":[-0.65674, -0.15479, 0.80933],"gyro":[10.62012, 3.23486, 5.92041]},{"cnt":1,"ms":16087,"acc":[-0.68066, -0.13013, 0.84131],"gyro":[14.58740, 1.15967, 4.88281]},{"cnt":2,"ms":16137,"acc":[-0.57373, -0.15991, 0.80200],"gyro":[4.94385, 10.07080, 10.13184]},{"cnt":3,"ms":16187,"acc":[-0.67090, -0.07959, 0.86743],"gyro":[1.89209, 10.37598, 11.16943]},{"cnt":4,"ms":16237,"acc":[-0.64697, -0.16943, 0.79370],"gyro":[-2.07520, -3.78418, 5.92041]},{"cnt":5,"ms":16287,"acc":[-0.64453, -0.05859, 0.73218],"gyro":[29.90723, 2.31934, 16.35742]},{"cnt":6,"ms":16337,"acc":[-0.60059, -0.11499, 0.83179],"gyro":[18.18848, -13.36670, 6.34766]},{"cnt":7,"ms":16387,"acc":[-0.74951, -0.14941, 0.81201],"gyro":[1.34277, -12.93945, 10.74219]},{"cnt":8,"ms":16437,"acc":[-0.60986, -0.09717, 0.79370],"gyro":[6.16455, -8.60596, 1.40381]},{"cnt":9,"ms":16487,"acc":[-0.59668, -0.07178, 0.89307],"gyro":[1.40381, -10.31494, 6.04248]}]}'
        + '}';
    const fetchedData = JSON.parse(fetchedJson);
    console.log("fetchedData", fetchedData)
    const newData = extractNewData(fetchedData);
    const dataArr = newData.dataArr;
    if (dataArr.length == 0) {
        sameDataCnt++;
        if (sameDataCnt > 5) {
            stopFetch();
            sameDataCnt = 0
        }

    } else {
        addNodes(dataArr, fetchedData.prev_data.delay, elm);
        intervalTime = fetchedData.latestData.delay * fetchedData.latestData.freq;
    }
}

function startFetch() {
    console.log("startFetch");
    if (!loopId) {
        document.querySelector("#clickButton").innerText = "stop";
        loopId = setInterval(operateNode, intervalTime);
        isFetching = !isFetching;
    }
}

function stopFetch() {
    console.log("stopfetch");
    if (loopId) {
        document.querySelector("#clickButton").innerText = "start";
        clearInterval(loopId);
        loopId = 0;
        isFetching = !isFetching;
    }
}


function clickHandler() {
    if (!isFetching) {
        startFetch();
    } else {
        stopFetch();
    }
}
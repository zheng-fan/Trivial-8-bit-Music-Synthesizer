function debug(x) {
    console.log(x);
}
F = 44100, CH = 2, DEP = 16, LVL = (1 << DEP - 1) - 1; //采样率，声道数，位深，采样点最大值
DEPbn = Math.round(DEP / 8);

function getWavHeader(LEN) //采样点个数
{
    var DATALEN = LEN * DEPbn * CH;
    var buf = new ArrayBuffer(44);
    var dv = new DataView(buf);
    dv.setInt8(0, 'R'.charCodeAt(0), true); //true表示小端字节序
    dv.setInt8(1, 'I'.charCodeAt(0), true);
    dv.setInt8(2, 'F'.charCodeAt(0), true);
    dv.setInt8(3, 'F'.charCodeAt(0), true);
    dv.setUint32(4, DATALEN + 44 - 8, true);
    dv.setInt8(8, 'W'.charCodeAt(0), true);
    dv.setInt8(9, 'A'.charCodeAt(0), true);
    dv.setInt8(10, 'V'.charCodeAt(0), true);
    dv.setInt8(11, 'E'.charCodeAt(0), true);
    dv.setInt8(12, 'f'.charCodeAt(0), true);
    dv.setInt8(13, 'm'.charCodeAt(0), true);
    dv.setInt8(14, 't'.charCodeAt(0), true);
    dv.setInt8(15, ' '.charCodeAt(0), true);
    dv.setUint32(16, 0x10, true);
    dv.setUint16(20, 1, true);
    dv.setUint16(22, CH, true);
    dv.setUint32(24, F, true);
    dv.setUint32(28, F * DEPbn * CH, true);
    dv.setUint16(32, DEPbn * CH, true);
    dv.setUint16(34, DEP, true);
    dv.setInt8(36, 'd'.charCodeAt(0), true);
    dv.setInt8(37, 'a'.charCodeAt(0), true);
    dv.setInt8(38, 't'.charCodeAt(0), true);
    dv.setInt8(39, 'a'.charCodeAt(0), true);
    dv.setUint32(40, DATALEN, true);
    return buf;
}

function getLthSamplingPointRaw(L, Ltot, Kf, WavFunc, VolumeFunc) {
    return Math.round(WavFunc((L / F) % (1 / Kf) / (1 / Kf)) * VolumeFunc(L / Ltot) * LVL);
}

function getKSampling(LEN, Kf, WavFunc, VolumeFunc) {
    var buf = new ArrayBuffer(LEN * DEPbn * CH);
    var dv = new DataView(buf);
    for (var i = 0; i < LEN; i++)
        for (var j = 0; j < CH; j++)
            //目前只考虑了位深为16的情况
            dv.setUint16((i * CH + j) * DEPbn, getLthSamplingPointRaw(i, LEN, Kf, WavFunc, VolumeFunc), true);
    return buf;
}

function getKSamplingBuf(LEN, Kf, buf, base, WavFunc, VolumeFunc) {
    var dv = new DataView(buf);
    for (var i = 0; i < LEN; i++)
        for (var j = 0; j < CH; j++)
            //目前只考虑了位深为16的情况
            dv.setUint16(base + (i * CH + j) * DEPbn, getLthSamplingPointRaw(i, LEN, Kf, WavFunc, VolumeFunc), true);
}

function norm(ret) {
    if (ret > 1) return 1;
    else if (ret < 0) return 0;
    return ret;
}

function WavFuncSin(x) {
    return norm((Math.sin(x * 2 * Math.PI) + 1) / 2);
}

function WavFuncPulse(x) {
    if (x < 0.5) return 0;
    else return 1;
}

function WavFuncTriangle(x) {
    x = norm(x);
    if (x < 0.5) return x;
    else return 1 - x;
}
RANDLEN = 5000;
var rand = new Array(RANDLEN);
for (var i = 0; i < RANDLEN; i++) rand[i] = Math.random();

function WavFuncNoise(x) {
    return rand[Math.round(x * (RANDLEN - 1))];
}

function VolumeFuncLine(x) {
    return 1;
}

function VolumeFuncSqrt(x) {
    x = norm(x);
    return norm(Math.sqrt(1 - x));
}

function VolumeFuncTridown(x) {
    x = norm(x);
    return norm(1 - x);
}

function TransformArrayBuffer(buf) {
    var dv = new DataView(buf);
    var s = new String;
    for (var i = 0; i < buf.byteLength; i++)
        s += String.fromCharCode(dv.getUint8(i))[0];
    return s;
}

function PlayBase64Wav(data) {
    var audio_obj = new Audio("data:audio/wav;base64," + data);
    audio_obj.play();
}
keyf = new Array(89);
keyf[48] = 440; //中央C是39，中央A是48
for (var i = 0; i < 88; i++)
    keyf[i] = keyf[48] * Math.pow(2, 1 / 12.0 * (i - 48));
keyf[88] = 0; //88为空
var keyname = new Array(
    "A-0", "A#0", "B-0",
    "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
    "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
    "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
    "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
    "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
    "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
    "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
    "C-8", "pause"
);
var noisename = new Array(
    "0-#", "1-#", "2-#", "3-#", "4-#", "5-#", "6-#", "7-#", "8-#", "9-#", "A-#", "B-#", "C-#", "D-#", "E-#", "F-#"
);
var mp = new Array();
for (var i = 0; i < keyname.length; i++) mp[keyname[i]] = i;
for (var i = 0; i < noisename.length; i++) mp[noisename[i]] = i * 5;

function trimArr(arr) {
    for (var i = 0; i < arr.length; i++) arr[i] = arr[i].trim();
}

function splitArr(arr) {
    for (var i = 0; i < arr.length; i++) {
        arr[i] = arr[i].split('/');
        arr[i][1] = parseFloat(arr[i][1]);
    }
}

function getLEN(arr, b2, LENpb) {
    var LEN = 0;
    for (var i = 0; i < arr.length; i++)
        LEN += Math.round(b2 / arr[i][1] * LENpb);
    return LEN;
}
var bpm = 72; //default
var b1 = 1;
var b2 = 4;

function getBuf(buf, tmp, arr, LENpb, WavFunc, VolumeFunc) {
    var base = 0;
    for (var i = 0; i < arr.length; i++) {
        var KLEN = Math.round(b2 / arr[i][1] * LENpb);
        getKSamplingBuf(KLEN, keyf[mp[arr[i][0]]], tmp, base, WavFunc, VolumeFunc);
        base += KLEN * CH * DEPbn;
    }
    var dv = new DataView(buf);
    var dvtmp = new DataView(tmp);
    for (var i = 0; i < Math.round(tmp.byteLength / DEPbn); i++)
        dv.setUint16(i * DEPbn, Math.round(dv.getUint16(i * DEPbn, true) + dvtmp.getUint16(i * DEPbn, true) / 4), true);
}

function getNotes(str, mxLEN) {
    var Pulse1 = str.match(/Pulse1:(.*)/)[1].split(',');
    var Pulse2 = str.match(/Pulse2:(.*)/)[1].split(',');
    var Triangle = str.match(/Triangle:(.*)/)[1].split(',');
    var Noise = str.match(/Noise:(.*)/)[1].split(',');
    trimArr(Pulse1);
    trimArr(Pulse2);
    trimArr(Triangle);
    trimArr(Noise);
    splitArr(Pulse1);
    splitArr(Pulse2);
    splitArr(Triangle);
    splitArr(Noise);
    var LENpb = Math.round(60 / bpm * F);
    var mxLEN = -1;
    mxLEN = Math.max(mxLEN, getLEN(Pulse1, b2, LENpb));
    mxLEN = Math.max(mxLEN, getLEN(Pulse2, b2, LENpb));
    mxLEN = Math.max(mxLEN, getLEN(Triangle, b2, LENpb));
    mxLEN = Math.max(mxLEN, getLEN(Noise, b2, LENpb));
    var buf = new ArrayBuffer(mxLEN * DEPbn * CH);
    var tmp = new ArrayBuffer(mxLEN * DEPbn * CH);
    //getBuf
    getBuf(buf, tmp, Pulse1, LENpb, WavFuncPulse, VolumeFuncSqrt);
    getBuf(buf, tmp, Pulse2, LENpb, WavFuncPulse, VolumeFuncSqrt);
    getBuf(buf, tmp, Triangle, LENpb, WavFuncTriangle, VolumeFuncLine);
    getBuf(buf, tmp, Noise, LENpb, WavFuncNoise, VolumeFuncTridown);

    return TransformArrayBuffer(buf);
}

// run it!

bpm = 230;
b1 = 1;
b2 = 4;

function play() {
    var LEN;
    var db = getNotes($('#score').val(), LEN);
    var dh = TransformArrayBuffer(getWavHeader(LEN));
    var d = dh + db;
    PlayBase64Wav(btoa(d));
}
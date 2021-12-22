"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.ML_UUID = exports.mluuid = void 0;
var fs_extra_1 = __importDefault(require("fs-extra"));
var path_1 = __importDefault(require("path"));
var crypto_1 = __importDefault(require("crypto"));
var byteToHex = [];
for (var i = 0; i < 256; ++i) {
    byteToHex[i] = (i + 0x100).toString(16).substr(1);
}
var mluuid = /** @class */ (function () {
    function mluuid() {
    }
    mluuid.prototype.bytesToUuid = function (buf, offset) {
        if (offset === void 0) { offset = 0; }
        var i = offset || 0;
        var bth = byteToHex;
        return [
            bth[buf[i++]],
            bth[buf[i++]],
            bth[buf[i++]],
            bth[buf[i++]],
            '-',
            bth[buf[i++]],
            bth[buf[i++]],
            '-',
            bth[buf[i++]],
            bth[buf[i++]],
            '-',
            bth[buf[i++]],
            bth[buf[i++]],
            '-',
            bth[buf[i++]],
            bth[buf[i++]],
            bth[buf[i++]],
            bth[buf[i++]],
            bth[buf[i++]],
            bth[buf[i++]],
        ].join('');
    };
    mluuid.prototype.v4 = function () {
        var rnds = crypto_1.default.randomBytes(16);
        rnds[6] = (rnds[6] & 0x0f) | 0x40;
        rnds[8] = (rnds[8] & 0x3f) | 0x80;
        return this.bytesToUuid(rnds);
    };
    mluuid.prototype.getUUID = function () {
        return this.v4();
    };
    return mluuid;
}());
exports.mluuid = mluuid;
exports.ML_UUID = new mluuid();
var input = process.argv[2];
var name = "assets/" + path_1.default.parse(input).base;
var uuid = exports.ML_UUID.getUUID();
var dupe = false;
while (!dupe) {
    if (!fs_extra_1.default.existsSync("./Rewards/" + uuid + ".json")) {
        dupe = true;
    }
    else {
        uuid = exports.ML_UUID.getUUID();
    }
}
var asset = { name: name, event: process.argv[3], category: process.argv[4], uuid: uuid };
var t = "./Rewards/" + uuid + ".json";
console.log(t);
fs_extra_1.default.writeFileSync(path_1.default.resolve(t), JSON.stringify(asset));

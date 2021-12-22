"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var fs_1 = __importDefault(require("fs"));
var path_1 = __importDefault(require("path"));
var child_process_1 = __importDefault(require("child_process"));
function getAllFiles(dir) {
    var files = [];
    fs_1.default.readdirSync(dir).forEach(function (f) {
        var file = path_1.default.resolve(dir, f);
        if (fs_1.default.existsSync(file) && !fs_1.default.lstatSync(file).isDirectory()) {
            files.push(file);
        }
    });
    return files;
}
getAllFiles("./Rewards/assets").forEach(function (file) {
    child_process_1.default.execSync("node ./generateTicket.js \"" + file + "\" \"Christmas 2020\" \"Unknown\"");
});
child_process_1.default.execSync("node ./organize.js");
getAllFiles("./Rewards").forEach(function (file) {
    var data = JSON.parse(fs_1.default.readFileSync(file).toString());
    if (data.name.indexOf("/oot/") > -1) {
        data["game"] = "OotO";
    }
    else if (data.name.indexOf("/mm/") > -1) {
        data["game"] = "MMO";
    }
    fs_1.default.writeFileSync(file, JSON.stringify(data));
});

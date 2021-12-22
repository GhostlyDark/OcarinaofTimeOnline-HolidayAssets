"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var fs_1 = __importDefault(require("fs"));
var path_1 = __importDefault(require("path"));
var subfolder = "mm";
fs_1.default.readdirSync("./Rewards").forEach(function (f) {
    var file = path_1.default.resolve("./Rewards", f);
    if (fs_1.default.existsSync(file) && !fs_1.default.lstatSync(file).isDirectory()) {
        var parse = path_1.default.parse(file);
        if (parse.ext === ".json") {
            var data = JSON.parse(fs_1.default.readFileSync(file).toString());
            if (data.category !== "Unknown")
                return;
            console.log("Parsing ticket " + f);
            var p = data.name;
            var t = path_1.default.resolve("./Rewards", p);
            if (fs_1.default.existsSync(t)) {
                console.log(t);
                var zobj = fs_1.default.readFileSync(t);
                var index = zobj.indexOf("MODLOADER64");
                var type = zobj.readUInt8(index + 0xB);
                switch (type) {
                    case 4 /* CHILD_MM */:
                    case 1 /* CHILD */:
                        fs_1.default.renameSync(t, path_1.default.resolve("./Rewards", "assets", subfolder, "child", path_1.default.parse(t).base));
                        data.name = "assets/" + subfolder + "/child/" + path_1.default.parse(t).base;
                        data.category = "Child";
                        break;
                    case 105 /* EQUIP */:
                        fs_1.default.renameSync(t, path_1.default.resolve("./Rewards", "assets", subfolder, "equipment", path_1.default.parse(t).base));
                        data.name = "assets/" + subfolder + "/equipment/" + path_1.default.parse(t).base;
                        data.category = "Equipment";
                        break;
                    case 104 /* ADULT_SIZED_MM */:
                    case 0 /* ADULT */:
                        fs_1.default.renameSync(t, path_1.default.resolve("./Rewards", "assets", subfolder, "adult", path_1.default.parse(t).base));
                        data.name = "assets/" + subfolder + "/adult/" + path_1.default.parse(t).base;
                        data.category = "Adult";
                        break;
                }
                fs_1.default.writeFileSync(file, JSON.stringify(data));
            }
        }
    }
});

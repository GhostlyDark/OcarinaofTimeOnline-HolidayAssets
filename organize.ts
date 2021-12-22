import fs from 'fs';
import path from 'path';

const enum TYPES {
    ADULT = 0x0,
    CHILD = 0x1,
    EQUIP = 0x69,
    ADULT_SIZED_MM = 0x68,
    CHILD_MM = 0x4,

}

let subfolder: string = "mm";

fs.readdirSync("./Rewards").forEach((f: string) => {
    let file = path.resolve("./Rewards", f);
    if (fs.existsSync(file) && !fs.lstatSync(file).isDirectory()) {
        let parse = path.parse(file);
        if (parse.ext === ".json") {
            let data = JSON.parse(fs.readFileSync(file).toString());
            if (data.category !== "Unknown") return;
            console.log("Parsing ticket " + f);
            let p = data.name;
            let t = path.resolve("./Rewards", p);
            if (fs.existsSync(t)) {
                console.log(t);
                let zobj = fs.readFileSync(t);
                let index = zobj.indexOf("MODLOADER64");
                let type = zobj.readUInt8(index + 0xB);
                switch (type) {
                    case TYPES.CHILD_MM:
                    case TYPES.CHILD:
                        fs.renameSync(t, path.resolve("./Rewards", "assets", subfolder, "child", path.parse(t).base));
                        data.name = "assets/" + subfolder + "/child/" + path.parse(t).base;
                        data.category = "Child";
                        break;
                    case TYPES.EQUIP:
                        fs.renameSync(t, path.resolve("./Rewards", "assets", subfolder, "equipment", path.parse(t).base));
                        data.name = "assets/" + subfolder + "/equipment/" + path.parse(t).base;
                        data.category = "Equipment";
                        break;
                    case TYPES.ADULT_SIZED_MM:
                    case TYPES.ADULT:
                        fs.renameSync(t, path.resolve("./Rewards", "assets", subfolder, "adult", path.parse(t).base));
                        data.name = "assets/" + subfolder + "/adult/" + path.parse(t).base;
                        data.category = "Adult";
                        break;
                }
                fs.writeFileSync(file, JSON.stringify(data));
            }
        }
    }
});
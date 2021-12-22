import fs from 'fs';
import path from 'path';
import child_process from 'child_process';

function getAllFiles(dir: string){
    let files: Array<string> = [];
    fs.readdirSync(dir).forEach((f: string)=>{
        let file = path.resolve(dir, f);
        if (fs.existsSync(file) && !fs.lstatSync(file).isDirectory()){
            files.push(file);
        }
    });
    return files;
}

getAllFiles("./Rewards/assets").forEach((file: string)=>{
    child_process.execSync("node ./generateTicket.js \"" + file + "\" \"Christmas 2020\" \"Unknown\"");
});

child_process.execSync("node ./organize.js");

getAllFiles("./Rewards").forEach((file: string)=>{
    let data = JSON.parse(fs.readFileSync(file).toString());
    if (data.name.indexOf("/oot/") > -1){
        data["game"] = "OotO";
    }else if (data.name.indexOf("/mm/") > -1){
        data["game"] = "MMO";
    }
    fs.writeFileSync(file, JSON.stringify(data));
});
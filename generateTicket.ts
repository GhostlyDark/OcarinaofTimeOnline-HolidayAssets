import fse from 'fs-extra';
import path from 'path';
import crypto from 'crypto';

const byteToHex: Array<string> = [];
for (var i = 0; i < 256; ++i) {
    byteToHex[i] = (i + 0x100).toString(16).substr(1);
}

export class mluuid {

    private bytesToUuid(buf: Buffer, offset: number = 0) {
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
    }

    private v4() {
        var rnds = crypto.randomBytes(16);
        rnds[6] = (rnds[6] & 0x0f) | 0x40;
        rnds[8] = (rnds[8] & 0x3f) | 0x80;
        return this.bytesToUuid(rnds);
    }

    getUUID(): string {
        return this.v4();
    }
}

export const ML_UUID: mluuid = new mluuid();

interface Asset {
    name: string;
    event: string;
    category: string;
    uuid: string;
}

let input: string = process.argv[2];

const name = "assets/" + path.parse(input).base;
let uuid = ML_UUID.getUUID();

let dupe = false;
while (!dupe) {
    if (!fse.existsSync("./Rewards/" + uuid + ".json")) {
        dupe = true;
    } else {
        uuid = ML_UUID.getUUID();
    }
}

let asset: Asset = { name, event: process.argv[3], category: process.argv[4], uuid: uuid };
let t = "./Rewards/" + uuid + ".json";
console.log(t);
fse.writeFileSync(path.resolve(t), JSON.stringify(asset));
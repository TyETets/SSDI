/* Tyler Ykema
 * 1062564
 * CIS2750 A4
 */

'use strict'

const mysql = require('mysql2/promise');
let connection;

// C library API
const ffi = require('ffi-napi');

let sharedLib = ffi.Library('./libsvgparse' , {
    'createValidSVGimage':['pointer', ['string', 'string']],
    'SVGtoJSON':['string', ['pointer']],
    'deleteSVGimage':['void',['pointer']],
    'validateSVGimage':['bool',['pointer', 'string']],
    'freeStr':['void', ['string']],
    'JSONcreator':['string',['pointer']],
    'editTitleDesc':['bool',['pointer', 'string', 'string']],
    'writeSVGimage':['bool',['pointer', 'string']],
    'getTitleAndDesc':['string', ['pointer']]
});

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************/
app.get('/uploads', function(req, res) {
    var files = fs.readdir('uploads/', function(err, files) {
        var strings = [files.length];
        if (err == null) {
            let j = 0;
            for (let i = 0; i < files.length; i++){
                let temp = path.join(__dirname+"/uploads/" + files[i]);
                let svgTemp = path.join(__dirname+"/parser/svg.xsd");
                let myObj = sharedLib.createValidSVGimage(temp, svgTemp);

                if (myObj != null) {
                    let myStr = sharedLib.SVGtoJSON(myObj);

                    if (myStr != '{}') {
                        strings[j] = JSON.parse(myStr);
                        strings[j]["fileName"] = files[i];
                        //console.log(myStr);

                        let stat = fs.statSync(temp);
                        let size = stat["size"];
                        size /= 1000;
                        strings[j]['size'] = size;

                        sharedLib.deleteSVGimage(myObj);

                        j++;
                    }
                }
            }

            var strArr = new Array(j);

            for(var i = 0; i < j; i++){
                strArr[i] = strings[i];
            }

            console.log('Loaded Images Successfully');
            res.json(strArr);
        } else {
            console.log('Error in loading images: ' + err);
            res.send('');
        }
    });
});

app.get('/view', function(req, res) {
    //let name = Request.Form['variable_name']
    var files = fs.readdir('uploads/', function(err, files) {
        if (err == null && files.length != 0) {
            //console.log(req.query.name);

            let svgTemp = path.join(__dirname+"/parser/svg.xsd");

            let name = req.query.name;
            var myObj;
            var result;

            let temp = path.join(__dirname+"/uploads/" + name);
            myObj = sharedLib.createValidSVGimage(temp, svgTemp);

            var jvar;

            if (myObj != null) {
                result = sharedLib.JSONcreator(myObj);
                //console.log(result);
                jvar = JSON.parse(result);
                if (jvar[8][0].title == "empty string" || jvar[8][0].title == ""){
                    jvar[8][0].title = "n/a";
                }
                if (jvar[8][1].description == "empty string" || jvar[8][1].description == ""){
                    jvar[8][1].description = "n/a";
                }
                sharedLib.deleteSVGimage(myObj);
            }

            res.json(jvar);
        }
        else {
            res.send('');
        }
    });
});

app.get('/edit', function(req, res) {
    //let name = Request.Form['variable_name']
    var files = fs.readdir('uploads/', function(err, files) {
        if (err == null) {
            let svgTemp = path.join(__dirname+"/parser/svg.xsd");

            let name = req.query.name;
            let title = req.query.title;
            let desc = req.query.desc;
            var myObj;
            var result;

            let temp = path.join(__dirname+"/uploads/" + name);
            myObj = sharedLib.createValidSVGimage(temp, svgTemp);

            var jvar;

            if (myObj != null) {
                var worked;
                if (title.length < 256 || desc.length < 256) {
                    worked = sharedLib.editTitleDesc(myObj, title, desc);

                    if (worked && sharedLib.validateSVGimage(myObj, svgTemp)) {
                        worked = sharedLib.writeSVGimage(myObj, "uploads/" + name);
                        if (worked) {
                            console.log("Successfully modified title/description");
                        }
                        else {
                            console.log("Failed to modify title/description");
                        }
                    }
                }
                else {
                    console.log("Error: string entered was too long");
                }
                result = sharedLib.JSONcreator(myObj);
                jvar = JSON.parse(result);
                if (jvar[8][0].title == "empty string" || jvar[8][0].title == ""){
                    jvar[8][0].title = "n/a";
                }
                if (jvar[8][1].description == "empty string" || jvar[8][1].description == ""){
                    jvar[8][1].description = "n/a";
                }
                sharedLib.deleteSVGimage(myObj);
            }

            res.json(jvar);
        }
        else {
            res.send('');
        }
    });
});

app.get('/loginDB', async function(req, res, next){
    var uName = req.query.Username;
    var password = req.query.Password;
    var dataBase = req.query.Database;

    //console.log(uName);
    //console.log(password);
    //console.log(dataBase);

    let connected = false;

    try {
        connection = await mysql.createConnection({
            host : 'dursley.socs.uoguelph.ca',
            user : uName,
            password : password,
            database : dataBase
        });

        console.log("Successful connection to database");

        connected = true;
    } catch(e) {
        console.log(e);
        connected = false;
    }

    if (connected == true) {
        try {
            await connection.execute("CREATE TABLE if not exists FILE (svg_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL, file_title VARCHAR(256), file_description VARCHAR(256), n_rect INT NOT NULL, n_circ INT NOT NULL, n_path INT NOT NULL, n_group INT NOT NULL, creation_time DATETIME NOT NULL, file_size INT NOT NULL, PRIMARY KEY(svg_id) )");
            await connection.execute("CREATE TABLE if not exists IMG_CHANGE (change_id INT AUTO_INCREMENT, change_type VARCHAR(256) NOT NULL, change_summary VARCHAR(256) NOT NULL, change_time DATETIME NOT NULL, svg_id INT NOT NULL, PRIMARY KEY(change_id), FOREIGN KEY(svg_id) REFERENCES FILE(svg_id) ON DELETE CASCADE)");
            await connection.execute("CREATE TABLE if not exists DOWNLOAD (download_id INT AUTO_INCREMENT, d_descr VARCHAR(256), svg_id INT NOT NULL, PRIMARY KEY(download_id), FOREIGN KEY(svg_id) REFERENCES FILE(svg_id) ON DELETE CASCADE)");
            console.log("Tables created successfully.");
        } catch (e) {
            console.log(e);
        }
    }

    res.send(connected);
});

app.get('/storedb', function(req, res) {
    //console.log("Storing files in database");
    var files = fs.readdir('uploads/', async function(err, files) {
        var strings = [files.length];

        if (err == null) {
            let j = 0;

            /*try {
                await connection.execute("DELETE FROM FILE");
                await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 0");
            } catch(e) {
                console.log(e);
            }*/

            for (let i = 0; i < files.length; i++){
                let temp = path.join(__dirname+"/uploads/" + files[i]);
                let svgTemp = path.join(__dirname+"/parser/svg.xsd");
                let myObj = sharedLib.createValidSVGimage(temp, svgTemp);

                if (myObj != null) {
                    let myStr = sharedLib.SVGtoJSON(myObj);
                    var titleStr = "n/a";
                    try {
                        titleStr = JSON.parse(sharedLib.getTitleAndDesc(myObj));
                    } catch(e) {
                        console.log(e);
                    }

                    if (myStr != '{}') {
                        strings[j] = JSON.parse(myStr);
                        strings[j]["fileName"] = files[i];
                        //console.log(myStr);

                        let stat = fs.statSync(temp);
                        let size = stat["size"];
                        size /= 1000;
                        strings[j]['size'] = size;

                        //console.log(stat['birthtime']);
                        strings[j]['dateCreated'] = stat['birthtime'];

                        strings[j]['title'] = titleStr['title'];
                        strings[j]['description'] = titleStr['description'];

                        sharedLib.deleteSVGimage(myObj);

                        j++;
                    }
                }
            }

            var strArr = new Array(j);
            var success;

            for(var i = 0; i < j; i++){
                strArr[i] = strings[i];

                var dataBaseFile;

                try {
                    let [rows, cols] = await connection.execute("SELECT file_name FROM FILE WHERE file_name = '" + strArr[i]['fileName'] + "'");
                    try {
                        //console.log(rows[0].file_name);
                        dataBaseFile = rows[0].file_name;
                    } catch (e) {
                        dataBaseFile = " ";
                    }
                } catch (e) {
                    console.log(e);
                    res.send(false);
                }

                //console.log(strArr[i]);

                if (dataBaseFile.localeCompare(strArr[i]['fileName']) != 0) {
                    let dateC = strArr[i]['dateCreated'];
                    let formattedDate = dateC.getFullYear() + "-" + (dateC.getMonth() + 1) + "-" + dateC.getDate() + " " + dateC.getHours() + ":" + dateC.getMinutes() + ":" + dateC.getSeconds();

                    try {
                        await connection.execute("INSERT INTO FILE VALUES (null, '" + strArr[i]['fileName']
                            + "', '" + strArr[i]['title'] + "', '" + strArr[i]['description'] + "', " + strArr[i]['numRect'] + ", "
                            + strArr[i]['numCirc'] + ", " + strArr[i]['numPaths'] + ", " + strArr[i]['numGroups'] + ", '" //+ ", NOW(), " + parseInt(strArr[i]['size']) + ")");
                            + formattedDate + "', " + strArr[i]['size'] + ");");

                        console.log('Stored ' + strArr[i]['fileName'] + ' successfully');
                        success = true;
                    } catch (e) {
                        console.log(e);
                        i = j; //exit loop
                        success = false;
                    }
                } else {
                    console.log("\tFile " + strArr[i].fileName + " already exists");

                    success = true;
                }
            }

            res.send(success);
        } else {
            console.log(err);
            res.send('');
        }
    });
});

app.get('/dbstatus', async function(req, res){
    var arr = {};
    let success = 0;

    try {
        let rows = await connection.execute("SELECT COUNT(*) FROM FILE");
        arr[0] = rows[0][0]['COUNT(*)'];
        success = 1;
        //console.log(rows[0][0]['COUNT(*)']);
    } catch (e) {
        success = 0;
        console.log(e);
    }

    try {
        let rows = await connection.execute("SELECT COUNT(*) FROM IMG_CHANGE");
        arr[1] = rows[0][0]['COUNT(*)'];
        success = 1;
    } catch (e) {
        success = 0;
        console.log(e);
    }

    try {
        let rows = await connection.execute("SELECT COUNT(*) FROM DOWNLOAD");
        arr[2] = rows[0][0]['COUNT(*)'];
        success = 1;
    } catch (e) {
        success = 0;
        console.log(e);
    }

    if (success = 1) {
        res.send(arr);
    } else {
        res.send(false);
    }
});

app.get('/cleardb', async function(req, res) {
    var success = false;

    try {
        await connection.execute("DELETE FROM IMG_CHANGE");
        await connection.execute("ALTER TABLE IMG_CHANGE AUTO_INCREMENT = 0");
        await connection.execute("DELETE FROM DOWNLOAD");
        await connection.execute("ALTER TABLE DOWNLOAD AUTO_INCREMENT = 0");
        await connection.execute("DELETE FROM FILE");
        await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 0");

        success = true;
    } catch(e) {
        console.log(e);
    }

    res.send(success);
});

app.get('/storedownload', async function(req, res) {
    let name = req.query.fileName;
    console.log("Storing download of " + name);

    var success = false;
    var svg_id = {};

    try {
        let rows = await connection.execute("SELECT svg_id FROM FILE WHERE file_name = '" + name + "' ");
        svg_id = rows[0][0]['svg_id'];
        //console.log(svg_id);
        success = true;
    } catch(e) {
        console.log(e);
    }

    if (success == true) {
        try {
            let rows = await connection.execute("INSERT INTO DOWNLOAD VALUES (null, '" + name + "', " + svg_id + " );");
        } catch (e) {
            console.log(e);
        }
    }

    res.send(success);
});

app.get('/q1', async function(req, res) {
    //console.log("Execute query 1. Sortby = " + req.query.sortby);
    let sort = req.query.sortby;

    let rows = {};

    if (sort == 0) {
        try {
            rows = await connection.execute("SELECT file_name, file_title, file_description, n_rect, n_circ, n_path, n_group, creation_time, file_size FROM FILE ORDER BY file_name;");
            console.log(rows[0]);
        } catch(e) {
            console.log(e);
        }
    } else if (sort == 1) {
        try {
            rows = await connection.execute("SELECT file_name, file_title, file_description, n_rect, n_circ, n_path, n_group, creation_time, file_size FROM FILE ORDER BY file_size DESC;");
            console.log(rows[0]);
        } catch(e) {
            console.log(e);
        }
    } else {
        console.log("Error in sort by selection");
    }

    res.send(rows[0]);
});

app.get('/q4', async function(req, res) {
    //console.log("Execute query 1. Sortby = " + req.query.sortby);
    let sort = req.query.sortby;
    let search = req.query.shapeStr.split(" ");
    let range = search[0].split("-");

    console.log(sort);

    var validsearch = false;

    if (search.length == 2 && range.length == 2 && !isNaN(range[0]) && !isNaN(range[1]) && parseInt(range[0]) <= parseInt(range[1])) {
        if (search[1].localeCompare("rectangles") == 0 || search[1].localeCompare("rects") == 0 || search[1].localeCompare("circles") == 0 || search[1].localeCompare("circs") == 0 || search[1].localeCompare("paths") == 0 || search[1].localeCompare("groups") == 0) {
            validsearch = true;
        }
    } else {
        validsearch = false;
    }
    console.log(validsearch);

    let rows = {};

    if (validsearch == true) {
        let searchShape = {};

        if (search[1].localeCompare("rectangles") == 0 || search[1].localeCompare("rects") == 0) {
            searchShape = "n_rect";
        } else if (search[1].localeCompare("circles") == 0 || search[1].localeCompare("circs") == 0) {
            searchShape = "n_circ";
        } else if (search[1].localeCompare("paths") == 0) {
            searchShape = "n_path";
        } else if (search[1].localeCompare("groups") == 0) {
            searchShape = "n_group";
        }

        console.log(searchShape);

        if (sort == 0) {
            try {
                rows = await connection.execute("SELECT file_name, file_title, file_description, n_rect, n_circ, n_path, n_group, creation_time, file_size FROM FILE WHERE " + searchShape + " >= " + range[0] + " AND " + searchShape + " <= " + range[1] + " ORDER BY file_name;");
                //console.log(rows[0]);
            } catch(e) {
                console.log(e);
            }
        } else if (sort == 1) {
            try {
                rows = await connection.execute("SELECT file_name, file_title, file_description, n_rect, n_circ, n_path, n_group, creation_time, file_size FROM FILE WHERE " + searchShape + " >= " + range[0] + " AND " + searchShape + " <= " + range[1] + " ORDER BY file_size DESC;");
                //console.log(rows[0]);
            } catch(e) {
                console.log(e);
            }
        } else if (sort == 2){
            try {
                rows = await connection.execute("SELECT file_name, file_title, file_description, n_rect, n_circ, n_path, n_group, creation_time, file_size FROM FILE WHERE " + searchShape + " >= " + range[0] + " AND " + searchShape + " <= " + range[1] + " ORDER BY " + searchShape + " DESC;");
                //console.log(rows[0]);
            } catch(e) {
                console.log(e);
            }
        } else {
            console.log("Error in sort by selection");
        }
    } else {
        console.log("User entry error");
        rows[0] = false;
    }

    //console.log(rows[0]);

    res.send(rows[0]);
});

app.get('/q5', async function(req, res) {
    let sort = req.query.sortby;
    let search = req.query.shapeStr;
    console.log(search);

    var validSearch = false;

    if (!isNaN(parseInt(search)) && search.split(" ").length == 1 && parseInt(search) > 0) {
        validSearch = true;
    } else {
        validSearch = false;
    }


    let rows = validSearch;

    if (validSearch == true) {
        var tmprows;
        try {
            tmprows = await connection.execute("SELECT d_descr, svg_id, COUNT(svg_id) c FROM DOWNLOAD GROUP BY d_descr ORDER BY c DESC LIMIT " + search + ";");
            console.log(tmprows[0]);
        } catch(e) {
            console.log(e);
        }

        let tmp = tmprows[0];

        console.log(tmp);

        if (sort == 0) {
            rows = tmp.sort((a, b) => (a.d_descr.toLowerCase() > b.d_descr.toLowerCase()) ? 1 : -1);
        } else if (sort == 1) {
            rows = tmprows[0];
        } else {
            console.log("Error in sort by selection");
        }
    }  else {
        console.log("User entry error");
        rows = validSearch;
    }

    res.send(rows);
});


//Sample endpoint
app.get('/someendpoint', function(req , res) {
  let retStr = req.query.name1 + " " + req.query.name2;
  res.send({
    foo: retStr
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

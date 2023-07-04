/* Tyler Ykema
 * 1062564
 * CIS2750 A3
 */

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/someendpoint',   //The server endpoint we are connecting to
        data: {
            name1: "Value 1",
            name2: "Value 2"
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, received string '"+data.foo+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data);

        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    $('form#uploadform').submit(function(e){
        e.preventDefault();
        var form = $('form')[0];
        var formData = new FormData(form);
        console.log(form);

        $.ajax({
            url: 'upload',
            data: formData,
            type: 'post',
            contentType: false,
            processData: false,
            success: function(data) {
                //console.log(data);
                $.ajax({
                    url: '/uploads',
                    dataType: 'json',
                    type: 'get',
                    success: function(data) {
                        let table = "<tr>\n";
                        table += "<th style=\"font-size:24px\" colspan=\"7\">File Log</th>\n"
                        table += "</tr>\n";
                        table += "<tr>\n";
                        table += "<td>Image (click to download)</td>\n";
                        table += "<td>File Name</td>\n";
                        table += "<td>File Size</td>\n";
                        table += "<td>Number of Rectangles</td>\n";
                        table += "<td>Number of Circles</td>\n";
                        table += "<td>Number of paths</td>\n";
                        table += "<td>Number of groups</td>\n";
                        table += "</tr>";
                        var drop;

                        $.each(data, function (i, k){
                            table += "<tr>\n";
                            table += "<source src=" + k.fileName + "type=\"image/svg\">\n";
                            table += "<td><a href=\"" + k.fileName + "\" download onclick=\"return downloadTrack(\'" + k.fileName + "\');\"><img src = \""+ k.fileName + "\"></a></td>\n";
                            table += "<td><a href=\"" + k.fileName + "\" download onclick=\"return downloadTrack(\'" + k.fileName + "\');\">" + k.fileName + "</a></td>\n";
                            let sizetmp = Math.round(k.size);
                            table += "<td>" + sizetmp + "kb</td>\n";
                            table += "<td>" + k.numRect + "</td>\n";
                            table += "<td>" + k.numCirc + "</td>\n";
                            table += "<td>" + k.numPaths + "</td>\n";
                            table += "<td>" + k.numGroups + "</td>\n";
                            table += "</tr>\n"
                            drop += "<option value=\"" + k.fileName + "\">" + k.fileName + "</option>";
                        });

                        if (data.length != 0) {
                            document.getElementById("storeButton").disabled = false;
                        }

                        $("#fileLogPanel").html(table);
                        $("#imageSelector").html(drop);
                        console.log("File log table loaded successfully");
                    },
                    fail: function(error) {
                        console.log(error);
                    }
                });
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    $('form#viewform').submit(function(e){
        e.preventDefault();
        let my_var = $('#imageSelector').val();
        //console.log(my_var);

        $.ajax({
            url: '/view',
            dataType: 'json',
            type: 'get',
            data: {name : my_var},
            success: function(data) {
                //console.log(data);
                let table = "<tr>\n" +
                "<th style=\"font-size:24px\" colspan=\"3\">SVG View Panel</th>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<td colspan=\"3\"><img src=\"" + my_var + "\" style=\"width:800px; height:auto;\"></td>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<th colspan=\"1\">Title</td>\n" +
                "<th colspan=\"2\">Description</td>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<td colspan=\"1\">" + data[8][0].title + "</td>\n" +
                "<td colspan=\"2\">" + data[8][1].description + "</td>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<td>Component</td>\n" +
                "<td>Summary</td>\n" +
                "<td>Other Attributes</td>\n" +
                "</tr>\n";

                $('#myModal').empty();

                $.each(data, function (i, ielement){
                    if (i == 0 ) { //rect
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Rectangle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Top left corner: x coordinate = " + data[i][j].x + data[i][j].units +
                                ", y coordinate = " + data[i][j].y + data[i][j].units +
                                ", width = " + data[i][j].w + data[i][j].units +
                                ", height = " + data[i][j].h + data[i][j].units +
                                "</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n" +
                                "</tr>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Rectangle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Top left corner: x coordinate = " + data[i][j].x + data[i][j].units +
                                ", y coordinate = " + data[i][j].y + data[i][j].units +
                                ", width = " + data[i][j].w + data[i][j].units +
                                ", height = " + data[i][j].h + data[i][j].units +
                                "</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Rmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n" +
                                "</tr>\n";

                                let modal = "<div id=\"Rmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                  "<div class=\"modal-header\">" +
                                    "<h4 class=\"modal-title\">Attributes of rectangle #"+ k +"</h4>" +
                                  "</div>" +
                                 "<div class=\"modal-body\">";

                                 $.each(data[1][j], function(x, elementx){
                                     modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                     modal += editButton();
                                 });


                                 modal += "</div>" +
                                  "<div class=\"modal-footer\">" +
                                    //editButton() +
                                    "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                  "</div>" +
                                "</div>" +
                             "</div>" +
                            "</div>";

                            $('#myModal').append(modal);
                            }
                        });
                    }
                    else if (i == 2) { //circ
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Circle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Center: x coordinate = " + data[i][j].cx + data[i][j].units +
                                ", y coordinate = " + data[i][j].cy + data[i][j].units +
                                ", radius = " + data[i][j].r + data[i][j].units +
                                "</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Circle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Center: x coordinate = " + data[i][j].cx + data[i][j].units +
                                ", y coordinate = " + data[i][j].cy + data[i][j].units +
                                ", radius = " + data[i][j].r + data[i][j].units +
                                "</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Cmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n";

                                let modal = "<div id=\"Cmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                "<div class=\"modal-header\">" +
                                "<h4 class=\"modal-title\">Attributes of circle #"+ k +"</h4>" +
                                "</div>" +
                                "<div class=\"modal-body\">";

                                $.each(data[3][j], function(x, elementx){
                                    modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                });


                                modal += "</div>" +
                                "<div class=\"modal-footer\">" +
                                editButton() +
                                //"<button type=\"button\" class=\"btn btn-default\">Edit</button>" +
                                "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                "</div>" +
                                "</div>" +
                                "</div>" +
                                "</div>";

                                $('#myModal').append(modal);
                            }
                        });
                    }
                    else if (i == 4 ) { //path
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Path " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Path data = " + data[i][j].d +
                                "</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Path " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Path data = " + data[i][j].d +
                                "</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Pmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n";

                                let modal = "<div id=\"Pmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                "<div class=\"modal-header\">" +
                                    "<h4 class=\"modal-title\">Attributes of path #"+ k +"</h4>" +
                                "</div>" +
                                "<div class=\"modal-body\">";

                                $.each(data[5][j], function(x, elementx){
                                    modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                });


                                modal += "</div>" +
                                "<div class=\"modal-footer\">" +
                                editButton() +
                                    "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                "</div>" +
                                "</div>" +
                                "</div>" +
                                "</div>";

                                $('#myModal').append(modal);
                            }
                        });
                    }
                    else if (i == 6) { //group
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Group " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">" + data[i][j].children +
                                " child elements</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Group " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">" + data[i][j].children +
                                " child elements</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Gmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n";

                                let modal = "<div id=\"Gmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                "<div class=\"modal-header\">" +
                                    "<h4 class=\"modal-title\">Attributes of group #"+ k +"</h4>" +
                                "</div>" +
                                "<div class=\"modal-body\">";

                                $.each(data[7][j], function(x, elementx){
                                    modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                });


                                modal += "</div>" +
                                "<div class=\"modal-footer\">" +
                                editButton() +
                                    "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                "</div>" +
                                "</div>" +
                                "</div>" +
                                "</div>";

                                $('#myModal').append(modal);
                            }
                        });
                    }
                });

                if (data.length != 0) {
                    document.getElementById("editTitle").disabled = false;
                }

                //console.log(table);

                $("#viewTable").html(table);
            },
            fail: function(error) {
                console.log("Server side failure: " + error);
            }
        });
    });

    $('form#edit').submit(function(e){
        e.preventDefault();
        let my_var = $('#imageSelector').val();
        let title = $('#titleT').val();
        let desc = $('#descT').val();
        //console.log("TITLE = " + title + "\nDESC = " + desc);

        $.ajax({
            url: '/edit',
            dataType: 'json',
            type: 'get',
            data: {name:my_var, title:title, desc:desc},
            success: function(data) {
                let table = "<tr>\n" +
                "<th style=\"font-size:24px\" colspan=\"3\">SVG View Panel</th>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<td colspan=\"3\"><img src=\"" + my_var + "\" style=\"width:800px; height:auto;\"></td>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<th colspan=\"1\">Title</td>\n" +
                "<th colspan=\"2\">Description</td>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<td colspan=\"1\">" + data[8][0].title + "</td>\n" +
                "<td colspan=\"2\">" + data[8][1].description + "</td>\n" +
                "</tr>\n" +
                "<tr>\n" +
                "<td>Component</td>\n" +
                "<td>Summary</td>\n" +
                "<td>Other Attributes</td>\n" +
                "</tr>\n";

                $.each(data, function (i, ielement){
                    if (i == 0 ) { //rect
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Rectangle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Top left corner: x coordinate = " + data[i][j].x + data[i][j].units +
                                ", y coordinate = " + data[i][j].y + data[i][j].units +
                                ", width = " + data[i][j].w + data[i][j].units +
                                ", height = " + data[i][j].h + data[i][j].units +
                                "</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n" +
                                "</tr>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Rectangle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Top left corner: x coordinate = " + data[i][j].x + data[i][j].units +
                                ", y coordinate = " + data[i][j].y + data[i][j].units +
                                ", width = " + data[i][j].w + data[i][j].units +
                                ", height = " + data[i][j].h + data[i][j].units +
                                "</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Rmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n" +
                                "</tr>\n";

                                let modal = "<div id=\"Rmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                  "<div class=\"modal-header\">" +
                                    "<h4 class=\"modal-title\">Attributes of rectangle #"+ k +"</h4>" +
                                  "</div>" +
                                 "<div class=\"modal-body\">";

                                 $.each(data[1][j], function(x, elementx){
                                     modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                 });


                                 modal += "</div>" +
                                  "<div class=\"modal-footer\">" +
                                  editButton() +
                                    "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                  "</div>" +
                                "</div>" +
                             "</div>" +
                            "</div>";

                            $('#myModal').append(modal);
                            }
                        });
                    }
                    else if (i == 2) { //circ
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Circle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Center: x coordinate = " + data[i][j].cx + data[i][j].units +
                                ", y coordinate = " + data[i][j].cy + data[i][j].units +
                                ", radius = " + data[i][j].r + data[i][j].units +
                                "</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Circle " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Center: x coordinate = " + data[i][j].cx + data[i][j].units +
                                ", y coordinate = " + data[i][j].cy + data[i][j].units +
                                ", radius = " + data[i][j].r + data[i][j].units +
                                "</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Cmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n";

                                let modal = "<div id=\"Cmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                "<div class=\"modal-header\">" +
                                    "<h4 class=\"modal-title\">Attributes of circle #"+ k +"</h4>" +
                                "</div>" +
                                "<div class=\"modal-body\">";

                                $.each(data[3][j], function(x, elementx){
                                    modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                });


                                modal += "</div>" +
                                "<div class=\"modal-footer\">" +
                                editButton() +
                                    "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                "</div>" +
                                "</div>" +
                                "</div>" +
                                "</div>";

                                $('#myModal').append(modal);
                            }
                        });
                    }
                    else if (i == 4 ) { //path
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Path " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Path data = " + data[i][j].d +
                                "</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Path " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">Path data = " + data[i][j].d +
                                "</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Pmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n";

                                let modal = "<div id=\"Pmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                "<div class=\"modal-header\">" +
                                    "<h4 class=\"modal-title\">Attributes of path #"+ k +"</h4>" +
                                "</div>" +
                                "<div class=\"modal-body\">";

                                $.each(data[5][j], function(x, elementx){
                                    modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                });


                                modal += "</div>" +
                                "<div class=\"modal-footer\">" +
                                    editButton() +
                                    "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                "</div>" +
                                "</div>" +
                                "</div>" +
                                "</div>";

                                $('#myModal').append(modal);
                            }
                        });
                    }
                    else if (i == 6) { //group
                        $.each(ielement, function (j, jelement){
                            let k = j + 1;
                            if (jelement.numAttr <= 0) {
                                table += "<tr>\n" +
                                "<td>Group " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">" + data[i][j].children +
                                " child elements</td>\n" +
                                "<td>" + data[i][j].numAttr + "</td>" +
                                "</td>\n";
                            }
                            else {
                                table += "<tr>\n" +
                                "<td>Group " + k + "</td>\n" +
                                "<td style=\"max-width:250px\">" + data[i][j].children +
                                " child elements</td>\n" +
                                "<td><button type=\"button\" class=\"btn btn-info btn-lg\" data-toggle=\"modal\" data-target=\"#Gmod" + k + "\">" + data[i][j].numAttr + "</button></td>" +
                                "</td>\n";

                                let modal = "<div id=\"Gmod" + k + "\" class=\"modal fade\" role=\"dialog\">" +
                                "<div class=\"modal-dialog\">" +
                                "<div class=\"modal-content\">" +
                                "<div class=\"modal-header\">" +
                                    "<h4 class=\"modal-title\">Attributes of group #"+ k +"</h4>" +
                                "</div>" +
                                "<div class=\"modal-body\">";

                                $.each(data[7][j], function(x, elementx){
                                    modal += "<p>Name: "+ elementx.name + ", Value: "+ elementx.value + "</p>";
                                });


                                modal += "</div>" +
                                "<div class=\"modal-footer\">" +
                                editButton() +
                                "<button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Exit</button>" +
                                "</div>" +
                                "</div>" +
                                "</div>" +
                                "</div>";

                                $('#myModal').append(modal);
                            }
                        });
                    }
                });

                //console.log(table);

                $("#viewTable").html(table);
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    $("form#login").submit(function(e){
        e.preventDefault();

        var username = document.getElementById("Username").value;
        var password = document.getElementById("Password").value;
        var database = document.getElementById("Database").value;

        var login;

        //console.log("Entered credentials" + " " + username + " " + password);// + " " + database);

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/loginDB',
            data: {Username: username, Password: password, Database: database},
            success: function (r) {
                console.log(r);
                if (r == true) {
                    login = "Welcome " + username + "\n<hr>\n";
                    let loginSuccess = "";
                    //hide login screen
                    $("#login").html(loginSuccess);
                    $("#displaylogin").html(login);
                    document.getElementById("everything").hidden = false;
                } else {
                    let loginSuccess = "<br>\n"
                        + "<div class=\"alert\">\n"
                        + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
                        + "Login failed. Please try again or contact you network administrator.\n"
                        + "</div>\n";

                    $("#loginSuccess").html(loginSuccess);
                }
            },
            fail: function(err) {
                login = "Sign in error.";
                console.log(err);
            }
        });
    });

    $("#storedb").submit(function(e){
        e.preventDefault();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/storedb',
            data: {},
            success: function (r) {
                if (r == true) {
                    console.log("Files successfully stored.");

                    /*let status = "<p></p>\n<div class=\"databasestatus\">\n"
                        + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
                        + "Database storage successful.\n";*/
                    getStatus();
                } else {
                    console.log("Error: could not store files.");

                    let status = "<p></p>\n<div class=\"alert\">\n"
                        + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
                        + "Database storage failed. Please reload the page.\n";

                    $("#showstatus").html(status);
                }
            },
            fail: function(e) {
                console.log(e);
            }
        });
    });

    $("#dbstatus").submit(function(e) {
        e.preventDefault();

        getStatus();
    });

    $("#cleardb").submit(function(e) {
        e.preventDefault();

        console.log("Clearing tables.");

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: 'cleardb',
            success: function(r) {
                console.log("Cleared tables.");

                var status = {};

                if (r == true)
                {
                    /*status = "<p></p>\n<div class=\"databasestatus\">\n"
                        + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
                        + "Database cleared.\n";*/
                    getStatus();
                } else {
                    status = "<p></p>\n<div class=\"alert\">\n"
                        + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
                        + "Clearing of database failed.\n";

                    $("#showstatus").html(status);
                }

            },
            fail: function(e) {
                console.log(e);
            }
        });
    });

    $("#q1").submit(function(e){
        e.preventDefault();

        let choice = -1;
        let input = document.getElementsByName("q1check");

        if (input[0].checked == true) {
            choice = 0;
        } else if (input[1].checked == true) {
            choice = 1;
        }

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: 'q1',
            data: {sortby: choice},
            success: function(r) {
                var table = "<br>\n<table border=1>\n";
                table += qTableHeader();


                for (let i = 0; i < r.length; i++) {
                    table += "<tr>\n<td>" + r[i].file_name + "</td>\n<td>" + r[i].file_title + "</td>\n<td>" + r[i].file_description + "</td>\n<td>" + r[i].n_rect + "</td>\n"
                    + "<td>" + r[i].n_circ + "</td>\n<td>" + r[i].n_path + "</td>\n<td>" + r[i].n_group + "</td>\n<td>" + r[i].creation_time + "</td>\n<td>" + r[i].file_size + "kb</td>\n</tr>\n";
                }

                if (r.length == 0) {
                    table += "<tr>\n<th colspan=\"9\">No files found in the database. Please try storing files.</th>\n</tr>\n";
                }

                table += "</table>\n";

                $("#q1table").html(table);
            },
            fail: function(e) {
                console.log(e);
            }
        });
    });

    $("#q4").submit(function(e){
        e.preventDefault();

        let choice = -1;
        let input = document.getElementsByName("q4check");
        let search = document.getElementById("q4text").value;

        if (input[0].checked == true) {
            choice = 0;
        } else if (input[1].checked == true) {
            choice = 1;
        } else if (input[2].checked == true) {
            choice = 2;
        }

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: 'q4',
            data: {
                sortby: choice,
                shapeStr: search
            },
            success: function(r) {
                var table;
                console.log(r);

                if (r != false) {
                    table = "<br>\n<table border=1>\n";
                    table += qTableHeader();

                    for (let i = 0; i < r.length; i++) {
                        table += "<tr>\n<td>" + r[i].file_name + "</td>\n<td>" + r[i].file_title + "</td>\n<td>" + r[i].file_description + "</td>\n<td>" + r[i].n_rect + "</td>\n"
                        + "<td>" + r[i].n_circ + "</td>\n<td>" + r[i].n_path + "</td>\n<td>" + r[i].n_group + "</td>\n<td>" + r[i].creation_time + "</td>\n<td>" + r[i].file_size + "kb</td>\n</tr>\n";
                    }

                    if (r.length == 0) {
                        table += "<tr>\n<th colspan=\"9\">No files found matching query.</th>\n</tr>\n";
                    }

                    table += "</table>\n";
                } else if (r == false) {
                    table = createPopUp("No data found. Please check your entered search and your database connection. (make sure you use a \"-\" between numbers and/or \"rectangles\", \"rects\", \"circles\", \"circs\", \"paths\" or \"groups\")\n");
                }
                $("#q4table").html(table);
            },
            fail: function(e) {
                console.log(e);
            }
        });
    });

    $("#q5").submit(function(e){
        e.preventDefault();

        let choice = -1;
        let input = document.getElementsByName("q5check");
        let search = document.getElementById("q5text").value;
        //console.log(search);
        //console.log(input[0].checked);

        console.log(input);

        if (input[0].checked == true) {
            choice = 0;
        } else if (input[1].checked == true) {
            choice = 1;
        }
        //console.log(choice);

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: 'q5',
            data: {
                sortby: choice,
                shapeStr: search
            },
            success: function(r) {
                var table;
                if (r != false) {
                    //console.log("Creating query 5 table");
                    table = "<br>\n<table border=1 width=\"25%\">\n";
                    table += "<tr>\n<th style=\"font-size:24px\" colspan=\"2\">Query 5 Result</th>\n</tr>\n"
                        + "<tr>\n<td>File name</td>\n<td>Amount of downloads</td>\n</tr>\n";

                    let length = 0;
                    if (search > r.length) {
                        length = r.length;
                    } else {
                        length = search;
                    }

                    for (let i = 0; i < length; i++) {
                        //console.log(r[i]);
                        table += "<tr>\n<td>" + r[i].d_descr + "</td>\n<td>" + r[i].c + "</td>\n</tr>\n";
                    }

                    if (r.length == 0) {
                        table += "<tr>\n<th colspan=\"2\">No files found matching query.</th>\n</tr>\n";
                    }

                    table += "</table>\n";
                } else  if (r == false) {
                    table = createPopUp("Entry error. Please make sure your entered search is correct. (make sure there is only one value and it is greater than 0, and the seach contains no letters/words)\n");
                }
                $("#q5table").html(table);
            },
            fail: function(e) {
                console.log(e);
            }
        });
    });

    //On page load, load all images and fill SVG view panel with data
    $.ajax({
        url: '/uploads',
        dataType: 'json',
        type: 'get',
        success: function(data) {
            let table = "<tr>\n";
            table += "<th style=\"font-size:24px\" colspan=\"7\">File Log</th>\n";
            table += "</tr>\n";
            table += "<tr>\n";
            table += "<td>Image (click to download)</td>\n";
            table += "<td>File Name</td>\n";
            table += "<td>File Size</td>\n";
            table += "<td>Number of Rectangles</td>\n";
            table += "<td>Number of Circles</td>\n";
            table += "<td>Number of paths</td>\n";
            table += "<td>Number of groups</td>\n";
            table += "</tr>";
            var drop;

            if (data.length != 0) {
                $.each(data, function (i, k){
                    table += "<tr>\n";
                    table += "<source src=" + k.fileName + "type=\"image/svg\">\n";
                    table += "<td><a href=\"" + k.fileName + "\" download onclick=\"return downloadTrack(\'" + k.fileName + "\');\"><img src = \""+ k.fileName + "\"></a></td>\n";
                    table += "<td><a href=\"" + k.fileName + "\" download onclick=\"return downloadTrack(\'" + k.fileName + "\');\">" + k.fileName + "</a></td>\n";
                    let sizetmp = Math.round(k.size);
                    table += "<td>" + sizetmp + "kb</td>\n";
                    table += "<td>" + k.numRect + "</td>\n";
                    table += "<td>" + k.numCirc + "</td>\n";
                    table += "<td>" + k.numPaths + "</td>\n";
                    table += "<td>" + k.numGroups + "</td>\n";
                    table += "</tr>\n";
                    drop += "<option value=\"" + k.fileName + "\">" + k.fileName + "</option>";
                });
            }
            else {
                table += "<tr>\n"
                table += "<td colspan=\"7\">No files have been uploaded yet</td>\n";
                table += "</tr>\n";
                var drop;
                drop += "<option value=\"none\">none</option>";
            }



            if (data.length == 0) {
                document.getElementById("storeButton").disabled = true;
                //document.getElementById("clearbutton").disabled = true;
                //document.getElementById("statusButton").disabled = true;
            }

            $("#fileLogPanel").html(table);
            $("#imageSelector").html(drop);
            console.log("File log table loaded successfully");
        },
        fail: function(error) {
            console.log(error);
            let table = "<tr>\n";
            table += "<th style=\"font-size:24px\">File Log</th>\n"
            table += "</tr>\n";
            table += "<tr>\n";
            table += "<td>No files have been uploaded yet</td>\n";
            table += "</tr>\n";
            var drop;
            drop += "<option value=\"none\">none</option>";

            $("#fileLogPanel").html(table);
            $("#imageSelector").html(drop);
        }
    });
});

function getStatus() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: 'dbstatus',
        success: function (r) {
            var status;

            if (Number.isInteger(r[0])) {
                status = "<p></p><div class=\"databasestatus\">\n"
                    + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
                    + "Database has " + r[0] + " files, " + r[1] + " changes, " + r[2] + " downloads\n";
            } else {
                status = "<p></p><div class=\"alert\">\n"
                    + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
                    + "Database connection failed. Please reload this page.";
            }

            $("#showstatus").html(status);
        },
        fail: function(e) {
            console.log(e);
        }
    });
}

function downloadTrack(file) {
    $.ajax({
        url: '/storedownload',
        dataType: 'json',
        type: 'get',
        data: {fileName: file},
        success: function(data) {
            console.log("Downloaded file: " + file + ".");
        },
        fail: function(error) {
            console.log(error);
        }
    });

    getStatus();
}

function createPopUp(string) {
    return "<p></p>\n<div class=\"alert\">\n"
        + "<span class=\"closebtn\" onclick=\"this.parentElement.style.display=\'none\';\">&times;</span>\n"
        + string + "\n";
}

function qTableHeader() {
    return "<tr>\n<th style=\"font-size:24px\" colspan=\"9\">Query 4 Result</th>\n</tr>\n"
        + "<tr>\n<td>File name</td>\n<td>Title</td>\n<td>Description</td>\n<td># of Rectangles</td>\n<td># of Circles</td>\n<td># of Paths</td>\n"
        + "<td># of Groups</td>\n<td>Creation date</td>\n<td>File Size</td>\n</tr>\n";
}

function enableShapes() {
    document.getElementById("q4text").disabled = false;
    document.getElementById("q4text").required = true;
}

function disableShapes() {
    document.getElementById("q4text").disabled = true;
    document.getElementById("q4text").value = "";
}

function editButton() {
    return "<button id=editButton type=\"button\" class=\"btn btn-default\">Edit</button>";
}

function openForm() {
  document.getElementById("myForm").style.display = "block";
}

function closeForm() {
  document.getElementById("myForm").style.display = "none";
}

function getCookie(name) {
    const value = `; ${document.cookie}`;
    const parts = value.split(`; ${name}=`);
    if (parts.length === 2) return parts.pop().split(';').shift();
}

function setCookie(cName, cValue, expDays) {
    let date = new Date();
    date.setTime(date.getTime() + (expDays * 24 * 60 * 60 * 1000));
    const expires = "expires=" + date.toUTCString();
    document.cookie = cName + "=" + cValue + "; " + expires + "; path=/";
}

window.onload = function() {
    let cookies = document.cookie;
    console.log(cookies);
    document.getElementById("connectedValue").innerHTML = getCookie("user");
};


function doComputation()
{

    document.getElementById("feur").innerHTML = document.getElementById("myInput").value;

    document.getElementById("newnameValue").innerHTML = "votre nouveau nom est : " + document.getElementById("newnameInput").value;
    document.getElementById("connectedValue").innerHTML = document.getElementById("newnameInput").value;
    setCookie("user", document.getElementById("newnameInput").value, 10000);


    document.getElementById("strlenValue").innerHTML = document.getElementById("strlenInput").value.length;
    document.getElementById("strcmpValue").innerHTML = document.getElementById("strcmpInput").value.localeCompare(document.getElementById("strcmpInput2").value);
}

$(document).on("click", "a.action", function(ev) {
    var test_type = "normal";
    $.post("start?" + test_type, function(data) {
        $.get("content-part.html?"+data, function(content) {
            $("div.content").html(content);
        });
    });
    ev.preventDefault();
});
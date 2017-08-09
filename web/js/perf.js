$(document).on("click", "a.action", function(ev) {
    var test_type = $(this).data().frame;
    $.post("start?frame=" + test_type, function(data) {
        if (test_type) {
            $("div.content").html("<iframe src=\"frame.html?"+data+"\"></iframe>");
        }
        else {
            $.get("content-part.html?"+data, function(content) {
                $("div.content").html(content);
            });
        }
    });
    ev.preventDefault();
});
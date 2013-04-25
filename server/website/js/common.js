$(document).ready(function(){
    bindBibtexToggle();
    bindDetailedDescriptionToggle();
    bindHelpExamplesToggle();
});

function bindBibtexToggle() {
    $('a.bibtex-toggler').click(function() {
        $(this).next().toggle('fast');
    });
}

function bindDetailedDescriptionToggle() {
    $('.help-toggler').click(function() {
        var button = $(this);
        button.prev().toggle('fast', function() {
            if ($(this).is(":visible")) {
                button.text('[show less]');
            }
            else {
                button.text('[show more]');
            }
        })
    });
}

function bindHelpExamplesToggle() {
    $('.example-toggler').click(function() {
        var button = $(this);
        button.parent().siblings('.help-example').toggle('fast', function() {
            if ($(this).is(":visible")) {
                button.text('[hide]');
            }
            else {
                button.text('[show]');
            }
        })
    });
}

function addRightMenu(menuId, containerId) {
    $('#' + menuId).append(createRightMenuFromHeaders(containerId));
    bindStickyRightMenu(menuId);
}

function createRightMenuFromHeaders(containerId) {
    var menu = $("<ul>");

    $('#' + containerId).children('h1,h2,h3,h4,h5,h6').each(function(index) {
        //if (index == 0) return true;

        if (typeof $(this).prop('tagName') != 'undefined') {
            var id = "header-" + index;
            var className = "right-menu-element-" + $(this).prop('tagName')

            $(this).attr('id', id);
            menu.append("<li class=\"" + className + "\"><a href=\"#" + id + "\">" +
                $(this).text() + "</a></li>");
        }
    });

    return menu.append("</ul>");
}

function bindStickyRightMenu(menuId) {
  var menu = $('#' + menuId);
  var origOffsetY = menu.offset().top;
  var extraTop = 12; // = .right-menu.sticky-menu#top
  var footerTop = $(document).height() - $("footer").outerHeight() - 2;

  function footerTopToDownOfPage() {
    return $(window).height() - ($("footer").offset().top - window.scrollY)
  }

  function onScroll(e) {
    if (window.scrollY + extraTop >= origOffsetY) {
        menu.addClass('sticky-menu');

        if (window.scrollY + extraTop + menu.height() > footerTop) {
            menu.css("top", "auto");
            menu.css("bottom", footerTopToDownOfPage() + "px");
        }
        else {
            menu.removeAttr("style");
        }
    }
    else {
        menu.removeAttr("style");
        menu.removeClass('sticky-menu');
    }
  }

  document.addEventListener('scroll', onScroll);
}

function createFAQList(listId, faqId) {
    var menu = $("<ul>");

    $('#' + faqId).children('h2').each(function(index) {
        if (typeof $(this).prop('tagName') != 'undefined') {
            var id = "faq-" + index;
            $(this).attr('id', id);

            menu.append("<li><a href=\"#" + id + "\">" + $(this).text() + "</a></li>");
        }
    });

    menu.append("</ul>");
    $('#' + listId).append(menu);
}

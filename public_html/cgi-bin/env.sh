#!/bin/bash 

# Capturez le contenu dans une variable
content=$(cat <<END
<html>
<h3>Environment:</h3>
<pre>
$(env)
</pre>
<h3>Hostname</h3>
<pre>
$(hostname)
</pre>
</html>
END
)

# Calculez la longueur du contenu
content_length=${#content}

# Imprimez les en-têtes HTTP et le contenu
echo -e "HTTP/1.1 200 OK"
echo -e "Content-Type: text/html"
echo -e "Content-Length: $content_length\r\n"
echo -e "$content\r\n"


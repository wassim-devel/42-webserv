
<!DOCTYPE html>
<html>
    <head>
        <title>PHP Test</title>
    </head>
    <body>
        <p>il faut set la variable coucou</p>
        <?php

         $welcome = "hello";
        if (isset($_SERVER['QUERY_STRING'])) {
            // Parse la chaîne de requête et stocke les résultats dans le tableau $params
            parse_str($_SERVER['QUERY_STRING'], $params);

            // Vérifie si 'coucou' est présent dans les paramètres parsés
            if (isset($params['coucou'])) {
                // Récupère la valeur de 'coucou'
                $coucou = $params['coucou'];
                echo "La valeur de 'coucou' est: " . $coucou;
            } else {
                echo "'coucou' n'est pas défini dans la chaîne de requête.";
            }
        } else {
            echo "La chaîne de requête QUERY_STRING n'est pas définie.";
        }


         ?>
    </body>
</html>

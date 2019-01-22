
isAdded()
{
    local filename="$1"
    local module="$2"
    grep -Eq '^[[:space:]]*AddModule[[:space:]]+"'"$module"'"[[:space:]]+"sd_'"$module"'"[[:space:]]+"'"$module"'.conf"[[:space:]]*' "$filename"
}

isDefaultModule()
{
    local filename="$1"
    local module="$2"
    grep -Eq '^[[:space:]]*DefaultModule[[:space:]]+'"$module"'[[:space:]]*' "$filename"
}

setDefaultModule()
{
    local filename="$1"
    local module="$2"    
    sed -i 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$module"'/' "$filename"
}

addModule()
{
    local filename="$1"
    local module="$2"
    sed -i -r 's/^[[:space:]]*#[[:space:]]*AddModule[[:space:]]+"'"$module"'"[[:space:]]+"sd_'"$module"'"[[:space:]]+"'"$module"'.conf[[:space:]]*"[[:space:]]*/AddModule "'"$module"'" "sd_'"$module"'" "'"$module"'.conf"/' "$filename"
}

removeModule()
{
    local filename="$1"
    local module="$2"
    sed -i -r 's/^[[:space:]]*AddModule[[:space:]]+"'"$module"'"[[:space:]]+"sd_'"$module"'"[[:space:]]+"'"$module"'.conf[[:space:]]*"[[:space:]]*/#AddModule "'"$module"'" "sd_'"$module"'" "'"$module"'.conf"/' "$filename"
}

function spd_conf_set() {
    local MODULE="$1"
    local FILE
    
    ls -t /home/*/.config/speech-dispatcher/speechd.conf /etc/speech-dispatcher/speechd.conf /usr/share/speech-dispatcher/conf/speechd.conf 2>>"$LOG" | while read FILE; do
		isAdded "$FILE" "$MODULE"
		if [ "$?" = "1" ]; then
			addModule "$FILE" "$MODULE"
		fi
		
		isDefaultModule "$FILE" "$MODULE"
		if [ "$?" = "1" ]; then
			setDefaultModule "$FILE" "$MODULE"
		fi
	done
}

function spd_conf_unset() {
    local MODULE="$1"
    local FILE
    
    ls -t /home/*/.config/speech-dispatcher/speechd.conf /etc/speech-dispatcher/speechd.conf /usr/share/speech-dispatcher/conf/speechd.conf 2>>"$LOG" | while read FILE; do
		removeModule "$FILE" "$MODULE"
    done
}


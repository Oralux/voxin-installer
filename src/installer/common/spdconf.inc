
LIST="/home/*/.config/speech-dispatcher/speechd.conf /etc/speech-dispatcher/speechd.conf /usr/share/speech-dispatcher/conf/speechd.conf"

isDefaultModule() {
    local filename="$1"
    local module="$2"
    grep -Eq '^[[:space:]]*DefaultModule[[:space:]]+'"$module"'[[:space:]]*' "$filename"
}

setDefaultModule() {
    local filename="$1"
    local module="$2"    
    sed -i 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$module"'/' "$filename"
}

function spd_conf_set() {
    local MODULE="$1"
    local FILE
    
    ls -t $LIST 2>>"$LOG" | while read FILE; do
		isDefaultModule "$FILE" "$MODULE"
		if [ "$?" = "1" ]; then
			setDefaultModule "$FILE" "$MODULE"
		fi
	done
}


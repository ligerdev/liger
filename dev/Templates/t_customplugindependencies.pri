QTC_PLUGIN_NAME = {{ plugin_name }}
QTC_LIB_DEPENDS += \
    {% for lib in lib_dependencies %}
    {% if lib_dependencies.index(lib) == lib_dependencies|length -1 %}
    {{ lib }} 
    {%else %}
    {{ lib }} \
    {% endif  %}
    {% endfor %}
QTC_PLUGIN_DEPENDS += \
    {% for plg in plugin_dependencies %}
    {% if plugin_dependencies.index(plg) == plugin_dependencies|length -1 %}
    {{ plg }} 
    {%else %}
    {{ plg }} \
    {% endif  %}
    {% endfor %}


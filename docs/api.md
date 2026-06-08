# Helix C/C++ client API (P4API) notes

## `m_ClientAPI.Run(const char *func, ClientUser *ui)`

The following commands and functionality does not contains in p4 API, client must implement their own:

- p4 set
- p4 aliases

## `ClientAPI::SetProtocol(const char *p, const char *v)`

`SetProtocol` sets **Helix protocol variables** before the connection is established. It must be called **before** `ClientAPI::Init()`.

The first string is the variable name; the second is its value. For flags that are only on/off, the value is often an empty string.

### Documented variables

| Name         | Meaning                                                                                                                                                                                                                      |
| ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `tag`        | Request **tagged** output when the server supports it for the command. Triggers `ClientUser::OutputStat()` instead of `OutputInfo()` for that data.                                                                          |
| `track`      | Request **tagged** output when the server supports it for the command. Triggers `ClientUser::OutputStat()` instead of `OutputInfo()` for that data.                                                                          |
| `specstring` | Enable **specstring** / form-descriptor handling for form-related commands. Commonly used together with `tag`.                                                                                                               |
| `api`        | Set the **protocol level** (e.g. `"55"`, `"56"`, `"57"`, `"58"`, …) to pin older client/server response behavior. Normally matches the API you linked; override only if you need compatibility with a specific release line. |

Example (tagged output, same idea as `p4 -z tag`):

```cpp
client.SetProtocol("tag", "");
client.Init(&e);
```

Example (forms with specstring + tag, as in the P4API user guide):

```cpp
client.SetProtocol("specstring", "");
client.SetProtocol("tag", "");
client.Init(&e);
```

Official reference: [ClientApi::SetProtocol](https://help.perforce.com/helix-core/apis/p4api/current/Content/P4API/clientapi.setprotocol.html)

### Other protocol keys

The `p4` client’s **`-z name`** and **`-z name=value`** options set protocol variables for the session. The table above is the small **documented** core. Newer server releases may add more variables; if the server accepts a name, you can set it the same way via `SetProtocol` before `Init()`.

In **p5**, global **`-z`** is parsed into name/value pairs and applied with `SetProtocol` during initialization (see `apply_protocol_options` and `P5::Initialize`).

## References

- [P4API 2025.1](https://help.perforce.com/helix-core/apis/p4api/2025.1/Content/P4API/Home-p4api.html)
- [Other version](https://help.perforce.com/helix-core/apis/p4api/current/Content/P4API/earlier-versions-p4api.html)

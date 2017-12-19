# pxscene Security Model

## Purpose
In addition to being a powerful scene graph application engine, pxscene also has the ability to provide applications with resources available on the underlying platform.  As access to these resources create potential vulnerabilities, the following security model is available to restrict access to those resources for applications with varying levels of trust.

## Permissions
In pxscene, permissions can be granted for the following resources:
- _url_ - the device's local web server.  A list of allowed urls for which an application has access. "*" may be used as a wild card.
- _serviceManager_ - A list of allowed services an application has access to. "*" indicates full access to all service manager services.  Service manager services do not have to be named with their version number.  For example, "com.comcast.application" is used rather than "com.comcast.application_1". This allows service versions to be updated without needing to update the permissions.
- _applications_ - a list of allowed or blocked wayland applications to which an application has access.  The name specified is what would be used within a pxApp to create the wayland component, e.g. scene.create({t:'wayland',cmd:'videoPlayer'})
- _features_ - one-off features that don't fall under service manager or localhost.  These are predefined.
  - _screenshot_ - indicates if the application can take a screenshot of the full scene graph.  Boolean.  true means that screenshots through scene.screenshot is accessible.

## Permission Types
Permissions may be of one of two types, allow and restrict.
- _allow_ - allows only the given resources
- _block_ - blocks only the given resources

Wildcards may be used to give a broad access of services.  Specific rules ( in which no wildcard is used ) should override generic rules where wild cards are used.

## Permissions Examples
These permissions are the least restrictive, allowing access to all resources.

```
{
  "url" : {
    "allow" : [ "*" ]
  },
 "serviceManager" : {
    "allow" : [ "*" ]
  },
 "applications" : {
    "allow" : [ "*" ]
  }, 
  "features" {
    "allow" : "screenshot"
  }
}
```

These permissions allow no service manager access, access to all URLs except for localhost, and access to all wayland applications except for the 'videoPlayer'.  Screenshot access is also not allowed. 

```
{
  "url" : {
    "allow" : [ "*" ],
     "block" : [ "http://localhost*" ]
  },
 "serviceManager" : {
    "block" : [ "*" ]
  },
 "applications" : {
    "allow" : ["*"],
    "block" : [ "videoPlayer" ]
  },
  "features" {
    "block" : "screenshot"
  }
}
```

In the example below, all URLs are allowed (the most generic rule) but access to all localhost resources other than authService/getDeviceId are blocked.  All service manager services, with exception of the system and deviceDiagnostics services, are allowed.  The application is allowed to launch the videoPlayer and webBrowser as wayland applications, but is blocked from launching any others.  Screenshots are also allowed.

```
{
  "url" : {
    "allow" : [
      "*",
      "http://localhost:1000/service1/getInfo"
    ],
    "block" : [ "http://localhost*" ]
  },
 "serviceManager" : {
    "allow" : [ "*" ],
    "block" : [
      "com.comcast.application",
      "com.comcast.stateObserver"
    ]
  },
 "applications" : {
    "allow" : [ "videoPlayer"],
    "allow" : [ "webBrowser" ],
    "block" : ["*"]
  },
  "features" {
    "allow" : "screenshot"
  }
}
```

## Specifying Application Permissions
scene.create uses the permissions property to assign permissions to an application.  These permissions can be assigned through one of two ways:

### Bootstrap Permissions, Roles and Assignments
The bootstrap permissions file defines roles, which are named permissions.  Role names are arbitrary, as are the permissions given to them.  In the example below, the "roles" object defines three roles: "fullTrust", "limitedTrust", and "untrusted".  Below the roles, the "assign" object assigns roles to url patterns.  A default bootstrap property is available on the device in /etc/permissions.json.

```
{
  "roles": {
 
    "fullAccess" : {
      {
        "url" : {
          "allow" : [ "*" ]
        },
       "serviceManager" : {
          "allow" : [ "*" ]
        },
        "applications" : {
            "allow" : ["*"]
        }, 
        "features" {
          "allow" : "screenshot"
        }
      }
    },
 
    "limitedTrust" : {
      "url" : {
        "allow" : [
          "*",
          "http://localhost:1000/service1/getInfo"
        ],
        "block" : [ "http://localhost*" ]
      },
     "serviceManager" : {
        "allow" : [ "*" ],
        "block" : [
          "com.comcast.application",
          "com.comcast.stateObserver"
        ]
      },
     "applications" : {
        "allow" : ["videoPlayer"],
        "allow" : ["webBrowser"],
        "allow" : ["hulu"],
        "block" : ["*"]
     }, 
      "features" {
          "allow" : "screenshot"
      }
    },
 
    "untrusted" : {
      {
        "url" : {
          "allow" : [ "*" ],
           "block" : [ "http://localhost*" ]
        },
       "serviceManager" : {
          "block" : [ "*" ]
         },
       "applications" : {
          "block" : ["*"]
         },
        "features" {
          "block" : "screenshot"
        }
      }   
    }
  }
 
 
  "assign" : {
    "https://*.comcast.com" : "fullTrust",
    "https://*.xcal.tv" : "fullTrust",
    "http://localhost:*" : "fullTrust",
    "https://*.partner1.com" : "fullTrust",
    "https://*.partner2.com" : "limitedTrust"
    "http://*" : "untrusted"
  }
}
```

In the example above, if a pxApp is created and permissions are not specified by the process creating the application, the bootstrap properties apply to the pxApp based on where it was loaded from.

### The Permissions Property

The permissions property can be used to pass a permission object as a construction property when creating scene through scene.create().  When using the permissions property, certain rules apply.  These are explained as follows:
- A parent application specifying permissions to a child application can only tighten the permissions on the child.  A parent cannot grant a child more permissions than a parent has.
- For url and serviceManager permissions, the child application will get permissions based on 
  - the intersection of the resources in the allows section, and
  - the union of the resources in the block section
  - if there is a conflict between the parent and child, the item in conflict will be blocked.
- For features, if the feature permission is in conflict, it goes to the most restrictive setting for that feature.

### Examples
```
var childPermissions = {
      "url" : {
        "allow" : [
          "*",
          "http://localhost:1000/service1/getInfo",
          "http://localhost:1000/service1/serviceApi2",
          "http://localhost:1000/service1/serviceApi3",
          "http://localhost:1000/service1/getServiceApi4",
        ],
        "block" : [ "http://localhost*" ]
      },
     "serviceManager" : {
        "allow" : [ "*" ],
        "block" : [
          "com.comcast.application",
          "com.comcast.stateObserver",
          "com.comcast.FrameRate"
        ]
      },
     "applications" : {
        "allow" : ["videoPlayer"]
     },
      "features" {
          "allow" : "screenshot"
      }
```
Assuming the Bootstrap permissions are in effect and "childPermissions" are defined as above.

Example Scenarios:
- A pxApp is created as a plugin resource in the XRE receiver and is given the URL "https://applications.comcast.com/foo/bar.js".  The application will be assigned to the "fullTrust" role, as it matches "https://*.comcast.com", based on the bootstrap permissions.
- A pxApp is created as a plugin resource in the XRE receiver and is given the URL "http://comcast.com/foo/bar.js".  The application will be assigned to the "untrusted" role, as it is loaded over http (but not localhost), based on the bootstrap permissions.
- A pxApp is loaded from "http://localhost:1000/webserver/app.js as a standalone application (it has the "fullTrust" role).  The app then creates a child app which has the URL "https://comcast.com/apps/childapp.js" and no permissions are provided when the child app is created.  Since no permissions are passed in, the child app will be assigned "fullTrust" as it matches the bootstrap property rule.
- A pxApp is loaded from "http://domain.com/parentapp.js" as a standalone application (it has the "untrusted" role based on bootstrap).  The app then creates a child app which has the URL "https://comcast.com/apps.childapp.js" and no permissions are provided.  In this case, the child app will be assigned to the "fullTrust" role, since no permissions were given at creation of the child, permissions fall on the bootstrap.
- A pxApp is loaded from "http://domain.com/parentapp.js" as a standalone application (it has the "untrusted" role).  The app then creates a child app which has the URL "http://otherdomain.com/" and passes childPermissions as the permissions property.  Since the parent app provided permissions, the result is an intersection of the allows and the union of the blocks.  For URLs, "*" would be allowed and "localhost*" would be blocked. For service manager APIs, all APIs would be blocked. 